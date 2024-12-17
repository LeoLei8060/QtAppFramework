#include "PluginManager.h"
#include <QDir>
#include <QFileSystemWatcher>
#include <QDebug>

namespace Core {

PluginManager* PluginManager::instance_ = nullptr;

PluginManager* PluginManager::instance()
{
    if (!instance_) {
        instance_ = new PluginManager();
    }
    return instance_;
}

PluginManager::PluginManager()
    : QObject(nullptr)
{
}

PluginManager::~PluginManager()
{
    // 卸载所有插件
    for (const QString& pluginName : loadedPlugins_.keys()) {
        unloadPlugin(pluginName);
    }

    // 清理加载器
    qDeleteAll(pluginLoaders_);
}

bool PluginManager::initialize(const QString& pluginsPath)
{
    pluginsPath_ = pluginsPath;
    QDir dir(pluginsPath_);
    
    // 确保插件目录存在
    if (!dir.exists() && !dir.mkpath(".")) {
        return false;
    }

    // 扫描插件目录
    scanPluginDirectory();
    return true;
}

bool PluginManager::loadPlugin(const QString& pluginName)
{
    // 检查插件是否已加载
    if (isPluginLoaded(pluginName)) {
        return true;
    }

    // 检查依赖
    QStringList missingDependencies;
    if (!checkPluginDependencies(pluginName, missingDependencies)) {
        QString error = QString("Missing dependencies: %1").arg(missingDependencies.join(", "));
        emit pluginError(pluginName, error);
        return false;
    }

    // 加载插件及其依赖
    QStringList loadedPlugins;
    if (!loadPluginWithDependencies(pluginName, loadedPlugins)) {
        return false;
    }

    return true;
}

bool PluginManager::unloadPlugin(const QString& pluginName)
{
    if (!isPluginLoaded(pluginName)) {
        return false;
    }

    // 检查是否有其他插件依赖于此插件
    for (auto* plugin : loadedPlugins_) {
        if (plugin->dependencies().contains(pluginName)) {
            QString error = QString("Plugin is required by other plugins");
            emit pluginError(pluginName, error);
            return false;
        }
    }

    // 卸载插件
    IPlugin* plugin = loadedPlugins_[pluginName];
    plugin->unload();
    loadedPlugins_.remove(pluginName);

    // 卸载插件库
    QPluginLoader* loader = pluginLoaders_[pluginName];
    if (!loader->unload()) {
        QString error = QString("Failed to unload plugin: %1").arg(loader->errorString());
        emit pluginError(pluginName, error);
        return false;
    }

    delete loader;
    pluginLoaders_.remove(pluginName);

    emit pluginUnloaded(pluginName);
    return true;
}

QStringList PluginManager::loadedPlugins() const
{
    return loadedPlugins_.keys();
}

QStringList PluginManager::availablePlugins() const
{
    QDir dir(pluginsPath_);
    QStringList filters;
#ifdef Q_OS_WIN
    filters << "*.dll";
#else
    filters << "*.so";
#endif
    return dir.entryList(filters, QDir::Files);
}

IPlugin* PluginManager::getPlugin(const QString& pluginName) const
{
    return loadedPlugins_.value(pluginName);
}

bool PluginManager::isPluginLoaded(const QString& pluginName) const
{
    return loadedPlugins_.contains(pluginName);
}

QStringList PluginManager::getPluginDependencies(const QString& pluginName) const
{
    if (IPlugin* plugin = getPlugin(pluginName)) {
        return plugin->dependencies();
    }
    return QStringList();
}

void PluginManager::setPluginDirectoryWatcher(bool enable)
{
    static QFileSystemWatcher* watcher = nullptr;
    
    if (enable && !watcher) {
        watcher = new QFileSystemWatcher(this);
        watcher->addPath(pluginsPath_);
        
        connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString&) {
            scanPluginDirectory();
        });
    } else if (!enable && watcher) {
        delete watcher;
        watcher = nullptr;
    }
}

bool PluginManager::loadPluginWithDependencies(const QString& pluginName, QStringList& loadedPlugins)
{
    // 防止循环依赖
    if (loadedPlugins.contains(pluginName)) {
        return true;
    }

    // 加载依赖
    QPluginLoader* loader = new QPluginLoader(getPluginPath(pluginName));
    QObject* instance = loader->instance();
    if (!instance) {
        QString error = QString("Failed to load plugin: %1").arg(loader->errorString());
        emit pluginError(pluginName, error);
        delete loader;
        return false;
    }

    IPlugin* plugin = qobject_cast<IPlugin*>(instance);
    if (!plugin) {
        QString error = "Invalid plugin interface";
        emit pluginError(pluginName, error);
        delete loader;
        return false;
    }

    // 加载依赖项
    for (const QString& dependency : plugin->dependencies()) {
        if (!loadPluginWithDependencies(dependency, loadedPlugins)) {
            delete loader;
            return false;
        }
    }

    // 初始化插件
    if (!plugin->initialize()) {
        QString error = "Plugin initialization failed";
        emit pluginError(pluginName, error);
        delete loader;
        return false;
    }

    pluginLoaders_[pluginName] = loader;
    loadedPlugins_[pluginName] = plugin;
    loadedPlugins.append(pluginName);
    
    emit pluginLoaded(pluginName);
    return true;
}

bool PluginManager::checkPluginDependencies(const QString& pluginName, QStringList& missingDependencies)
{
    QPluginLoader loader(getPluginPath(pluginName));
    QObject* instance = loader.instance();
    if (!instance) {
        return false;
    }

    IPlugin* plugin = qobject_cast<IPlugin*>(instance);
    if (!plugin) {
        return false;
    }

    bool result = true;
    for (const QString& dependency : plugin->dependencies()) {
        if (!QFile::exists(getPluginPath(dependency))) {
            missingDependencies.append(dependency);
            result = false;
        }
    }

    loader.unload();
    return result;
}

void PluginManager::scanPluginDirectory()
{
    QStringList currentPlugins = availablePlugins();
    
    // 检查是否有插件被删除
    for (const QString& pluginName : loadedPlugins_.keys()) {
        if (!currentPlugins.contains(pluginName)) {
            unloadPlugin(pluginName);
        }
    }

    // 尝试加载新插件
    for (const QString& pluginName : currentPlugins) {
        if (!isPluginLoaded(pluginName)) {
            loadPlugin(pluginName);
        }
    }
}

QString PluginManager::getPluginPath(const QString& pluginName) const
{
    return QDir(pluginsPath_).filePath(pluginName);
}

} // namespace Core
