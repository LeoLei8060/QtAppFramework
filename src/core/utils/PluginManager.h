#pragma once

#include "IPlugin.h"
#include <QObject>
#include <QMap>
#include <QPluginLoader>
#include <memory>

namespace Core {

/**
 * @brief 插件管理器
 * 提供以下功能：
 * 1. 插件加载和卸载
 * 2. 插件依赖管理
 * 3. 插件版本控制
 * 4. 插件热插拔
 */
class PluginManager : public QObject
{
    Q_OBJECT
public:
    static PluginManager* instance();

    /**
     * @brief 初始化插件管理器
     * @param pluginsPath 插件目录
     */
    bool initialize(const QString& pluginsPath);

    /**
     * @brief 加载插件
     * @param pluginName 插件名称
     */
    bool loadPlugin(const QString& pluginName);

    /**
     * @brief 卸载插件
     * @param pluginName 插件名称
     */
    bool unloadPlugin(const QString& pluginName);

    /**
     * @brief 获取已加载的插件列表
     */
    QStringList loadedPlugins() const;

    /**
     * @brief 获取可用的插件列表
     */
    QStringList availablePlugins() const;

    /**
     * @brief 获取插件实例
     * @param pluginName 插件名称
     */
    IPlugin* getPlugin(const QString& pluginName) const;

    /**
     * @brief 检查插件是否已加载
     * @param pluginName 插件名称
     */
    bool isPluginLoaded(const QString& pluginName) const;

    /**
     * @brief 获取插件依赖
     * @param pluginName 插件名称
     */
    QStringList getPluginDependencies(const QString& pluginName) const;

    /**
     * @brief 监视插件目录变化
     * @param enable 是否启用
     */
    void setPluginDirectoryWatcher(bool enable);

signals:
    void pluginLoaded(const QString& pluginName);
    void pluginUnloaded(const QString& pluginName);
    void pluginError(const QString& pluginName, const QString& error);

private:
    PluginManager();
    ~PluginManager() override;

    static PluginManager* instance_;
    QString pluginsPath_;
    QMap<QString, QPluginLoader*> pluginLoaders_;
    QMap<QString, IPlugin*> loadedPlugins_;
    
    bool loadPluginWithDependencies(const QString& pluginName, QStringList& loadedPlugins);
    bool checkPluginDependencies(const QString& pluginName, QStringList& missingDependencies);
    void scanPluginDirectory();
    QString getPluginPath(const QString& pluginName) const;
};

} // namespace Core
