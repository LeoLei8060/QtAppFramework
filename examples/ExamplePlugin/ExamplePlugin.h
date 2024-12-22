#pragma once

#include "../../src/core/plugin/IPlugin.h"

class ExamplePlugin : public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_IID)
    Q_INTERFACES(IPlugin)

public:
    explicit ExamplePlugin(QObject *parent = nullptr);
    ~ExamplePlugin() override = default;

    // IPlugin 基本接口
    bool        initialize() override;
    QString     name() const override;
    QString     version() const override;
    QString     description() const override;
    QStringList supportedInterfaces() const override;

    // 数据处理接口
    bool canProcessData() const override { return true; }
    QString processData(const QString &data) override;

private:
    bool m_initialized;
};
