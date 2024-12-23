#include "TrafficLightPlugin.h"
#include "TrafficLightWidget.h"
#include <QDebug>

TrafficLightPlugin::TrafficLightPlugin(QObject *parent)
    : IPlugin(parent)
    , m_initialized(false)
{}

bool TrafficLightPlugin::initialize()
{
    if (!m_initialized) {
        m_initialized = true;
        qDebug() << "TrafficLightPlugin initialized";
    }
    return m_initialized;
}

QString TrafficLightPlugin::name() const
{
    return "TrafficLightPlugin";
}

QString TrafficLightPlugin::version() const
{
    return "1.0.0";
}

QString TrafficLightPlugin::description() const
{
    return "A traffic light widget plugin";
}

QStringList TrafficLightPlugin::supportedInterfaces() const
{
    return {"UI"};
}

QWidget *TrafficLightPlugin::createWidget(QWidget *parent)
{
    return new TrafficLightWidget(parent);
}
