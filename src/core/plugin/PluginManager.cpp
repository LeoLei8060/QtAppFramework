#include "PluginManager.h"
#include "../../version.h"
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

PluginManager *PluginManager::s_instance = nullptr;

PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{}

PluginManager::~PluginManager()
{
    unloadPlugins();
}

PluginManager *PluginManager::instance()
{
    if (!s_instance) {
        s_instance = new PluginManager();
    }
    return s_instance;
}

bool PluginManager::checkVersion(const QString &pluginVersion) const
{
    // 解析系统版本
    QString     systemVersion = QString(PROJECT_VERSION);
    QStringList sysParts = systemVersion.split('.');
    if (sysParts.size() < 2)
        return false;
    int sysMajor = sysParts[0].toInt();
    int sysMinor = sysParts[1].toInt();

    // 解析插件版本
    QStringList pluginParts = pluginVersion.split('.');
    if (pluginParts.size() < 2)
        return false;
    int pluginMajor = pluginParts[0].toInt();
    int pluginMinor = pluginParts[1].toInt();

    // 主版本号必须相同，插件次版本号必须小于等于系统次版本号
    return (sysMajor == pluginMajor) && (pluginMinor <= sysMinor);
}

bool PluginManager::loadPlugin(const QString &path)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        emit pluginLoadFailed(fileInfo.fileName(), "Plugin file does not exist");
        return false;
    }

    // 创建插件加载器
    auto loader = new QPluginLoader(path);

    // 检查插件是否已经加载
    QString pluginIID = loader->metaData()["IID"].toString();
    if (pluginIID.isEmpty()) {
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(), "Invalid plugin: no IID found");
        return false;
    }

    // 检查是否已加载相同 IID 的插件
    for (auto existingLoader : m_loaders) {
        if (existingLoader->metaData()["IID"].toString() == pluginIID) {
            delete loader;
            qDebug() << "Plugin with IID" << pluginIID << "already loaded";
            return true;
        }
    }

    // 加载插件
    if (!loader->load()) {
        QString error = loader->errorString();
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(), error);
        return false;
    }

    // 获取插件实例
    QObject *instance = loader->instance();
    if (!instance) {
        loader->unload();
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(), "Failed to create plugin instance");
        return false;
    }

    // 转换为插件接口
    IPlugin *plugin = qobject_cast<IPlugin *>(instance);
    if (!plugin) {
        loader->unload();
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(), "Not a valid plugin");
        return false;
    }

    // 检查版本兼容性
    if (!checkVersion(plugin->version())) {
        loader->unload();
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(),
                              QString("Plugin version %1 is not compatible with system version %2")
                                  .arg(plugin->version(), PROJECT_VERSION));
        return false;
    }

    // 初始化插件
    if (!plugin->initialize()) {
        loader->unload();
        delete loader;
        emit pluginLoadFailed(fileInfo.fileName(), "Plugin initialization failed");
        return false;
    }

    // 保存插件和加载器
    m_plugins[plugin->name()] = plugin;
    m_loaders[plugin->name()] = loader;

    emit pluginLoaded(plugin->name());
    return true;
}

void PluginManager::unloadPlugins()
{
    // 先删除所有插件实例
    qDeleteAll(m_plugins);
    m_plugins.clear();

    // 卸载并删除所有加载器
    for (auto loader : m_loaders) {
        loader->unload();
        delete loader;
    }
    m_loaders.clear();
}

IPlugin *PluginManager::plugin(const QString &name) const
{
    return m_plugins.value(name);
}

QList<IPlugin *> PluginManager::getUIPlugins() const
{
    QList<IPlugin *> result;
    for (auto plugin : m_plugins) {
        if (plugin->hasWidget()) {
            result.append(plugin);
        }
    }
    return result;
}

QList<IPlugin *> PluginManager::getDataProcessorPlugins() const
{
    QList<IPlugin *> result;
    for (auto plugin : m_plugins) {
        if (plugin->canProcessData()) {
            result.append(plugin);
        }
    }
    return result;
}

QList<IPlugin *> PluginManager::getDataProviderPlugins() const
{
    QList<IPlugin *> result;
    for (auto plugin : m_plugins) {
        if (plugin->canProvideData()) {
            result.append(plugin);
        }
    }
    return result;
}

QList<IPlugin *> PluginManager::getDataAcceptorPlugins() const
{
    QList<IPlugin *> result;
    for (auto plugin : m_plugins) {
        if (plugin->canAcceptData()) {
            result.append(plugin);
        }
    }
    return result;
}

QList<IPlugin *> PluginManager::getExecutablePlugins() const
{
    QList<IPlugin *> result;
    for (auto plugin : m_plugins) {
        if (plugin->canExecute()) {
            result.append(plugin);
        }
    }
    return result;
}
