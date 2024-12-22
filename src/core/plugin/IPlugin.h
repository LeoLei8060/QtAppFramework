#pragma once

#include <QObject>
#include <QString>
#include <QWidget>
#include <QtPlugin>

class IPlugin : public QObject
{
    Q_OBJECT

public:
    // 插件界面位置枚举
    enum class WidgetPosition {
        TopToolBar,   // 顶部工具栏
        LeftSideBar,  // 左侧边栏
        RightSideBar, // 右侧边栏
        CentralArea,  // 中央区域
        BottomArea,   // 底部区域
        StatusBar     // 状态栏
    };

    explicit IPlugin(QObject *parent = nullptr)
        : QObject(parent)
    {}
    ~IPlugin() override = default;

    // 基本接口
    virtual bool        initialize() = 0;
    virtual QString     name() const = 0;
    virtual QString     version() const = 0;
    virtual QString     description() const = 0;
    virtual QStringList supportedInterfaces() const = 0;

    // UI 相关接口
    virtual bool hasWidget() const { return false; }
    virtual QWidget* createWidget(QWidget *parent = nullptr) { Q_UNUSED(parent); return nullptr; }
    virtual WidgetPosition widgetPosition() const { return WidgetPosition::CentralArea; }

    // 数据处理接口
    virtual bool canProcessData() const { return false; }
    virtual QString processData(const QString &data) { Q_UNUSED(data); return QString(); }

    // 数据获取接口
    virtual bool canProvideData() const { return false; }
    virtual QVariant getData(const QString &key) { Q_UNUSED(key); return QVariant(); }

    // 数据设置接口
    virtual bool canAcceptData() const { return false; }
    virtual bool setData(const QString &key, const QVariant &value) { Q_UNUSED(key); Q_UNUSED(value); return false; }

    // 执行接口
    virtual bool canExecute() const { return false; }
    virtual bool execute(const QString &command, const QVariantMap &params = QVariantMap()) 
    { 
        Q_UNUSED(command); 
        Q_UNUSED(params); 
        return false; 
    }

signals:
    void dataChanged(const QString &key, const QVariant &value);
    void executionCompleted(const QString &command, bool success, const QVariant &result);
    void error(const QString &errorMessage);
};

// 声明插件接口
#define PLUGIN_IID "com.qtappframework.plugin.IPlugin"
Q_DECLARE_INTERFACE(IPlugin, PLUGIN_IID)
