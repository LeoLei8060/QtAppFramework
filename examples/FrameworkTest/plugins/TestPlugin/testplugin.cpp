#include "testplugin.h"
#include <QDebug>

TestPlugin::TestPlugin()
    : isLoaded_(false)
{
}

TestPlugin::~TestPlugin()
{
    if (isLoaded_) {
        unload();
    }
}

bool TestPlugin::initialize()
{
    qDebug() << "TestPlugin initializing...";
    isLoaded_ = true;
    return true;
}

void TestPlugin::unload()
{
    qDebug() << "TestPlugin unloading...";
    isLoaded_ = false;
}

QVariant TestPlugin::execute(const QString& command, const QVariantMap& params)
{
    qDebug() << "TestPlugin executing command:" << command << "with params:" << params;
    
    if (command == "test") {
        return "TestPlugin executed successfully";
    }
    
    return QVariant();
}
