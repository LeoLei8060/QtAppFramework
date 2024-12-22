#include "mainwindow.h"
#include "../../src/core/plugin/PluginManager.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QLibrary>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConnections();
    updatePluginState();
}

MainWindow::~MainWindow() {}

void MainWindow::onLoadPlugin()
{
    QString pluginPath = QFileDialog::getOpenFileName(this,
                                                      tr("Load Plugin"),
                                                      "plugins",
                                                      tr("Plugin Files (*.dll *.so *.dylib)"));

    if (pluginPath.isEmpty()) {
        return;
    }

    PluginManager::instance()->loadPlugin(QFileInfo(pluginPath).absoluteFilePath());
}

void MainWindow::loadPluginsFromDirectory(const QString &path)
{
    QDir       pluginsDir(path);
    const auto entries = pluginsDir.entryList(QDir::Files);

    for (const QString &fileName : entries) {
        QString filePath = pluginsDir.absoluteFilePath(fileName);
        if (QLibrary::isLibrary(filePath)) {
            PluginManager::instance()->loadPlugin(filePath);
        }
    }
}

void MainWindow::onUnloadPlugin()
{
    if (!m_pluginWidgets.isEmpty()) {
        // 移除所有插件控件
        for (const auto &name : m_pluginWidgets.keys()) {
            removePluginWidget(name);
        }
        m_pluginWidgets.clear();

        // 清除所有插件
        m_plugins.clear();

        // 卸载所有插件
        PluginManager::instance()->unloadPlugins();

        updatePluginState();
        updatePluginList();
    }
}

void MainWindow::onAnalyzeText()
{
    QString text = ui->textEdit->toPlainText();

    // 遍历所有支持数据处理的插件
    auto analyzers = PluginManager::instance()->getDataProcessorPlugins();
    for (auto plugin : analyzers) {
        QString result = plugin->processData(text);
        if (!result.isEmpty()) {
            ui->resultLabel->setText(result);
            break;
        }
    }
}

void MainWindow::onPluginLoaded(const QString &name)
{
    auto plugin = PluginManager::instance()->plugin(name);
    if (!plugin) {
        return;
    }

    // 保存插件指针
    m_plugins[name] = plugin;

    // 如果插件支持UI，添加其控件
    if (plugin->hasWidget()) {
        addPluginWidget(plugin);
    }

    updatePluginState();
    updatePluginList();
}

void MainWindow::onPluginLoadFailed(const QString &name, const QString &error)
{
    QMessageBox::warning(this,
                         tr("Plugin Load Failed"),
                         tr("Failed to load plugin %1: %2").arg(name, error));
}

void MainWindow::setupConnections()
{
    connect(ui->btnLoadPlugin, &QPushButton::clicked, this, &MainWindow::onLoadPlugin);
    connect(ui->btnUnloadPlugin, &QPushButton::clicked, this, &MainWindow::onUnloadPlugin);
    connect(ui->btnAnalyze, &QPushButton::clicked, this, &MainWindow::onAnalyzeText);

    connect(PluginManager::instance(),
            &PluginManager::pluginLoaded,
            this,
            &MainWindow::onPluginLoaded);
    connect(PluginManager::instance(),
            &PluginManager::pluginLoadFailed,
            this,
            &MainWindow::onPluginLoadFailed);
}

void MainWindow::updatePluginState()
{
    // 只要有任何插件加载，就激活 unload 按钮
    bool hasPlugins = !m_plugins.isEmpty();
    ui->btnUnloadPlugin->setEnabled(hasPlugins);
    ui->btnAnalyze->setEnabled(hasPlugins);
    ui->textEdit->setEnabled(hasPlugins);

    if (!hasPlugins) {
        ui->resultLabel->clear();
    }
}

void MainWindow::addPluginWidget(IPlugin *plugin)
{
    if (!plugin || !plugin->hasWidget()) {
        return;
    }

    auto widget = plugin->createWidget();
    if (!widget) {
        return;
    }

    // 根据插件指定的位置添加控件
    switch (plugin->widgetPosition()) {
    case IPlugin::WidgetPosition::TopToolBar:
        if (auto toolbar = findChild<QToolBar *>()) {
            toolbar->addWidget(widget);
        }
        break;
    case IPlugin::WidgetPosition::LeftSideBar:
        if (auto layout = findChild<QVBoxLayout *>("leftLayout")) {
            layout->addWidget(widget);
        }
        break;
    case IPlugin::WidgetPosition::RightSideBar:
        if (auto layout = findChild<QVBoxLayout *>("rightLayout")) {
            layout->addWidget(widget);
        }
        break;
    case IPlugin::WidgetPosition::CentralArea:
        if (auto layout = findChild<QVBoxLayout *>("centralLayout")) {
            layout->addWidget(widget);
        }
        break;
    case IPlugin::WidgetPosition::BottomArea:
        if (auto layout = findChild<QVBoxLayout *>("bottomLayout")) {
            layout->addWidget(widget);
        }
        break;
    case IPlugin::WidgetPosition::StatusBar:
        statusBar()->addWidget(widget);
        break;
    default:
        // 默认添加到工具栏旁边
        ui->horizontalLayout->insertWidget(ui->horizontalLayout->indexOf(ui->btnUnloadPlugin) + 1,
                                           widget);
        break;
    }

    m_pluginWidgets[plugin->name()] = widget;
}

void MainWindow::removePluginWidget(const QString &name)
{
    auto it = m_pluginWidgets.find(name);
    if (it != m_pluginWidgets.end()) {
        delete it.value();
        m_pluginWidgets.remove(name);
    }
}

void MainWindow::updatePluginList()
{
    ui->pluginList->clear();
    for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
        auto    plugin = it.value();
        QString info = QString("%1 v%2 - %3")
                           .arg(plugin->name())
                           .arg(plugin->version())
                           .arg(plugin->description());
        ui->pluginList->addItem(info);
    }
}
