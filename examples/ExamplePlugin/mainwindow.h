#pragma once

#include <QMainWindow>
#include <memory>
#include <QMap>
#include "../../src/core/plugin/IPlugin.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class IPlugin;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadPlugin();
    void onUnloadPlugin();
    void onAnalyzeText();
    void onPluginLoaded(const QString &name);
    void onPluginLoadFailed(const QString &name, const QString &error);

private:
    void setupConnections();
    void updatePluginState();
    void addPluginWidget(IPlugin* plugin);
    void removePluginWidget(const QString& name);
    void updatePluginList();
    void loadPluginsFromDirectory(const QString& path);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QMap<QString, IPlugin*> m_plugins;
    QMap<QString, QWidget*> m_pluginWidgets;
};
