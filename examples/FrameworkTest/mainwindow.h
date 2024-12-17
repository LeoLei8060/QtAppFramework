#pragma once

#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // 配置管理测试
    void onSaveConfig();
    void onLoadConfig();
    void onConfigChanged(const QString &key, const QVariant &value);

    // 事件系统测试
    void onPublishEvent();
    void onSubscribeEvent();
    void On_Event_TestEvent(const QString &message);

    // 网络请求测试
    void onSendRequest();
    void onRequestFinished();

    // 主题管理测试
    void onThemeChanged(const QString &themeName);
    void onCreateTheme();
    void onApplyTheme(const QString &themeName);

    // 插件测试
    void onLoadPlugin();
    void onUnloadPlugin();
    void onPluginLoaded(const QString &pluginName);
    void onPluginUnloaded(const QString &pluginName);

    // 单例应用测试组
    void onSendMessage();
    void onMessageReceived(const QString &message);

private:
    void setupUI();
    void initializeFramework();
    void createDefaultTheme();
    void createDefaultConfig();

    Ui::MainWindow *ui;

    // UI 组件
    QTextEdit   *logText;
    QComboBox   *themeCombo;
    QPushButton *sendEventBtn;
    QPushButton *sendRequestBtn;
    QPushButton *loadPluginBtn;
    QLabel      *statusLabel;
    QPushButton *sendMessageBtn;

    void log(const QString &message);
};
