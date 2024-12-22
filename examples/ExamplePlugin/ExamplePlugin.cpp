#include "ExamplePlugin.h"
#include <QDebug>

ExamplePlugin::ExamplePlugin(QObject *parent)
    : IPlugin(parent)
    , m_initialized(false)
{}

bool ExamplePlugin::initialize()
{
    if (!m_initialized) {
        m_initialized = true;
        qDebug() << "ExamplePlugin initialized";
    }
    return m_initialized;
}

QString ExamplePlugin::name() const
{
    return "ExamplePlugin";
}

QString ExamplePlugin::version() const
{
    return "1.0.0";
}

QString ExamplePlugin::description() const
{
    return "A text analysis plugin";
}

QStringList ExamplePlugin::supportedInterfaces() const
{
    return {"DataProcessor"};
}

QString ExamplePlugin::processData(const QString &data)
{
    return QString("Text length: %1 characters").arg(data.length());
}
