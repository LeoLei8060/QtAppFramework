#pragma once

#include "core/utils/IPlugin.h"
#include <QObject>

class TestPlugin : public QObject, public Core::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PLUGIN_IID FILE "TestPlugin.json")
    Q_INTERFACES(Core::IPlugin)

public:
    TestPlugin();
    ~TestPlugin() override;

    QString name() const override { return "TestPlugin"; }
    QString version() const override { return "1.0.0"; }
    QString description() const override { return "A test plugin for QAppFramework"; }
    
    bool initialize() override;
    void unload() override;
    bool isLoaded() const override { return isLoaded_; }
    QStringList dependencies() const override { return QStringList(); }
    
    QVariant execute(const QString& command, const QVariantMap& params = {}) override;

private:
    bool isLoaded_;
};
