#pragma once

#include "../../../src/core/plugin/IPlugin.h"

class TrafficLightWidget;

class TrafficLightPlugin : public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "TrafficLight.plugin")
    Q_INTERFACES(IPlugin)

public:
    explicit TrafficLightPlugin(QObject *parent = nullptr);
    ~TrafficLightPlugin() override = default;

    // IPlugin 基本接口
    bool        initialize() override;
    QString     name() const override;
    QString     version() const override;
    QString     description() const override;
    QStringList supportedInterfaces() const override;

    // UI 相关接口
    bool           hasWidget() const override { return true; }
    QWidget       *createWidget(QWidget *parent = nullptr) override;
    WidgetPosition widgetPosition() const override { return WidgetPosition::TopToolBar; }

private:
    bool m_initialized;
};
