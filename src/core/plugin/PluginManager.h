#pragma once

#include <QObject>
#include <QMap>
#include <QString>
#include <QPluginLoader>
#include "IPlugin.h"

class PluginManager : public QObject
{
    Q_OBJECT

public:
    static PluginManager* instance();

    bool loadPlugin(const QString& path);
    void unloadPlugins();
    IPlugin* plugin(const QString& name) const;
    QList<IPlugin*> plugins() const { return m_plugins.values(); }

    // 获取支持特定功能的插件
    QList<IPlugin*> getUIPlugins() const;
    QList<IPlugin*> getDataProcessorPlugins() const;
    QList<IPlugin*> getDataProviderPlugins() const;
    QList<IPlugin*> getDataAcceptorPlugins() const;
    QList<IPlugin*> getExecutablePlugins() const;

signals:
    void pluginLoaded(const QString& name);
    void pluginLoadFailed(const QString& name, const QString& error);
    void pluginUnloaded(const QString& name);

private:
    explicit PluginManager(QObject* parent = nullptr);
    ~PluginManager() override;

    static PluginManager* s_instance;
    QMap<QString, IPlugin*> m_plugins;
    QMap<QString, QPluginLoader*> m_loaders;
};
