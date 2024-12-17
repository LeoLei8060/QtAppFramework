#include "mainwindow.h"
#include "core/event/EventManager.h"
#include "core/utils/ConfigManager.h"
#include "core/utils/PluginManager.h"
#include "core/utils/SingleApplication.h"
#include "gui/style/ThemeManager.h"
#include "network/NetworkManager.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

using namespace Core;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(nullptr)
{
    setupUI();
    initializeFramework();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 事件测试组
    QGroupBox   *eventGroup = new QGroupBox("事件系统测试", this);
    QVBoxLayout *eventLayout = new QVBoxLayout(eventGroup);
    sendEventBtn = new QPushButton("发送测试事件", this);
    eventLayout->addWidget(sendEventBtn);
    mainLayout->addWidget(eventGroup);

    // 网络测试组
    QGroupBox   *networkGroup = new QGroupBox("网络请求测试", this);
    QVBoxLayout *networkLayout = new QVBoxLayout(networkGroup);
    sendRequestBtn = new QPushButton("发送HTTP请求", this);
    networkLayout->addWidget(sendRequestBtn);
    mainLayout->addWidget(networkGroup);

    // 主题测试组
    QGroupBox   *themeGroup = new QGroupBox("主题管理测试", this);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);
    themeCombo = new QComboBox(this);
    QPushButton *createThemeBtn = new QPushButton("创建新主题", this);
    themeLayout->addWidget(themeCombo);
    themeLayout->addWidget(createThemeBtn);
    mainLayout->addWidget(themeGroup);

    // 插件测试组
    QGroupBox   *pluginGroup = new QGroupBox("插件系统测试", this);
    QVBoxLayout *pluginLayout = new QVBoxLayout(pluginGroup);
    loadPluginBtn = new QPushButton("加载插件", this);
    pluginLayout->addWidget(loadPluginBtn);
    mainLayout->addWidget(pluginGroup);

    // 单例应用测试组
    QGroupBox   *singletonGroup = new QGroupBox("单例应用测试", this);
    QVBoxLayout *singletonLayout = new QVBoxLayout(singletonGroup);
    sendMessageBtn = new QPushButton("发送消息到其他实例", this);
    singletonLayout->addWidget(sendMessageBtn);
    mainLayout->addWidget(singletonGroup);

    // 日志显示
    QGroupBox   *logGroup = new QGroupBox("日志", this);
    QVBoxLayout *logLayout = new QVBoxLayout(logGroup);
    logText = new QTextEdit(this);
    logText->setReadOnly(true);
    logLayout->addWidget(logText);
    mainLayout->addWidget(logGroup);

    // 状态栏
    statusLabel = new QLabel(this);
    statusBar()->addWidget(statusLabel);

    // 连接信号槽
    connect(sendEventBtn, &QPushButton::clicked, this, &MainWindow::onPublishEvent);
    connect(sendRequestBtn, &QPushButton::clicked, this, &MainWindow::onSendRequest);
    connect(createThemeBtn, &QPushButton::clicked, this, &MainWindow::onCreateTheme);
    connect(loadPluginBtn, &QPushButton::clicked, this, &MainWindow::onLoadPlugin);
    connect(themeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onApplyTheme);
    connect(sendMessageBtn, &QPushButton::clicked, this, &MainWindow::onSendMessage);
}

void MainWindow::initializeFramework()
{
    QString appDir = QCoreApplication::applicationDirPath();

    // 初始化配置管理器
    auto *config = ConfigManager::instance();
    if (!config->initialize(appDir + "/config/app.json", appDir + "/config/user.json")) {
        log("配置管理器初始化失败");
    }
    createDefaultConfig();
    connect(config, &ConfigManager::configChanged, this, &MainWindow::onConfigChanged);

    // 初始化事件管理器
    EventManager::instance();
    onSubscribeEvent();

    // 初始化网络管理器
    auto *network = NetworkManager::instance();
    network->initialize();
    network->addRequestInterceptor(
        [this](QNetworkRequest &request) { log("请求拦截器: " + request.url().toString()); });

    // 初始化主题管理器
    auto *theme = ThemeManager::instance();
    if (!theme->initialize(appDir + "/themes")) {
        log("主题管理器初始化失败");
    }
    createDefaultTheme();
    connect(theme, &ThemeManager::themeChanged, this, &MainWindow::onThemeChanged);

    // 更新主题下拉框
    themeCombo->addItems(theme->availableThemes());

    // 初始化插件管理器
    auto *plugin = PluginManager::instance();
    if (!plugin->initialize(appDir + "/plugins")) {
        log("插件管理器初始化失败");
    }
    connect(plugin, &PluginManager::pluginLoaded, this, &MainWindow::onPluginLoaded);
    connect(plugin, &PluginManager::pluginUnloaded, this, &MainWindow::onPluginUnloaded);

    log("框架初始化完成");
}

void MainWindow::createDefaultConfig()
{
    auto *config = ConfigManager::instance();

    // 应用配置
    config->setValue("app.name", "Framework Test");
    config->setValue("app.version", "1.0.0");

    // 用户配置
    config->setValue("user.theme", "default", ConfigManager::User);

    config->saveConfig(ConfigManager::App);
    config->saveConfig(ConfigManager::User);
}

void MainWindow::createDefaultTheme()
{
    auto *theme = ThemeManager::instance();

    if (!theme->availableThemes().contains("default")) {
        ThemeManager::instance()->createCustomTheme("default", "");
        theme->setTheme("default");
    }
}

void MainWindow::onSaveConfig()
{
    ConfigManager::instance()->saveConfig();
    log("配置已保存");
}

void MainWindow::onLoadConfig()
{
    ConfigManager::instance()->reloadConfig();
    log("配置已重新加载");
}

void MainWindow::onConfigChanged(const QString &key, const QVariant &value)
{
    log(QString("配置已更改: %1 = %2").arg(key, value.toString()));
}

void MainWindow::onPublishEvent()
{
    QString message = QString("测试事件 - %1").arg(QDateTime::currentDateTime().toString());
    EventManager::instance()->postEvent("TestEvent", message);
    log("已发送事件: " + message);
}

void MainWindow::onSubscribeEvent()
{
    EventManager::instance()->postEventWithArgs("TestEvent", "Hello from direct call");
}

void MainWindow::On_Event_TestEvent(const QString &message)
{
    log("收到事件: " + message);
}

void MainWindow::onSendRequest()
{
    auto          *network = NetworkManager::instance();
    QNetworkReply *reply = network->get("https://api.github.com/zen");

    connect(reply, &QNetworkReply::finished, this, &MainWindow::onRequestFinished);
    log("发送HTTP请求...");
}

void MainWindow::onRequestFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QString response = QString::fromUtf8(reply->readAll());
            log("收到响应: " + response);
        } else {
            log("请求错误: " + reply->errorString());
        }
        reply->deleteLater();
    }
}

void MainWindow::onThemeChanged(const QString &themeName)
{
    log("主题已更改为: " + themeName);
    statusLabel->setText("当前主题: " + themeName);
}

void MainWindow::onCreateTheme()
{
    QString newThemeName = "custom_"
                           + QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
    if (ThemeManager::instance()->createCustomTheme(newThemeName, "default")) {
        themeCombo->addItem(newThemeName);
        log("已创建新主题: " + newThemeName);
    }
}

void MainWindow::onApplyTheme(const QString &themeName)
{
    if (ThemeManager::instance()->setTheme(themeName)) {
        log("已应用主题: " + themeName);
    }
}

void MainWindow::onLoadPlugin()
{
    QStringList availablePlugins = PluginManager::instance()->availablePlugins();
    if (availablePlugins.isEmpty()) {
        log("没有可用的插件");
        return;
    }

    QString pluginName = availablePlugins.first();
    if (PluginManager::instance()->loadPlugin(pluginName)) {
        log("插件加载成功: " + pluginName);
        loadPluginBtn->setText("卸载插件");
        disconnect(loadPluginBtn, &QPushButton::clicked, this, &MainWindow::onLoadPlugin);
        connect(loadPluginBtn, &QPushButton::clicked, this, &MainWindow::onUnloadPlugin);
    } else {
        log("插件加载失败: " + pluginName);
    }
}

void MainWindow::onUnloadPlugin()
{
    QStringList loadedPlugins = PluginManager::instance()->loadedPlugins();
    if (loadedPlugins.isEmpty()) {
        log("没有已加载的插件");
        return;
    }

    QString pluginName = loadedPlugins.first();
    if (PluginManager::instance()->unloadPlugin(pluginName)) {
        log("插件已卸载: " + pluginName);
        loadPluginBtn->setText("加载插件");
        disconnect(loadPluginBtn, &QPushButton::clicked, this, &MainWindow::onUnloadPlugin);
        connect(loadPluginBtn, &QPushButton::clicked, this, &MainWindow::onLoadPlugin);
    } else {
        log("插件卸载失败: " + pluginName);
    }
}

void MainWindow::onPluginLoaded(const QString &pluginName)
{
    log("插件已加载: " + pluginName);
}

void MainWindow::onPluginUnloaded(const QString &pluginName)
{
    log("插件已卸载: " + pluginName);
}

void MainWindow::onSendMessage()
{
    log("尝试发送消息到其他实例...");
    auto app = qobject_cast<Core::SingleApplication *>(QApplication::instance());
    if (app) {
        if (app->sendMessage("show")) {
            log("消息已发送到其他实例");
        } else {
            log("没有其他实例在运行");
        }
    }
}

void MainWindow::onMessageReceived(const QString &message)
{
    log("收到来自其他实例的消息: " + message);
    if (message == "show") {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::log(const QString &message)
{
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logText->append(QString("[%1] %2").arg(timeStamp, message));
}
