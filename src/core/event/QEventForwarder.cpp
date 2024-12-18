#include "QEventForwarder.h"
#include <QWriteLocker>

namespace Core {

QMap<QByteArray, QList<QPointer<QObject>>> QEventForwarder::m_eventPool;
QReadWriteLock QEventForwarder::m_lock;
QString QEventForwarder::m_lastError;

void QEventForwarder::unsubscribe(QObject *listener, const QByteArray &eventName)
{
    if (!listener || eventName.isEmpty()) {
        return;
    }

    QWriteLocker locker(&m_lock);
    if (!m_eventPool.contains(eventName)) {
        return;
    }

    auto &listeners = m_eventPool[eventName];
    for (auto it = listeners.begin(); it != listeners.end();) {
        if (!it->data() || it->data() == listener) {
            it = listeners.erase(it);
        } else {
            ++it;
        }
    }

    // 如果没有监听器了，删除这个事件
    if (listeners.isEmpty()) {
        m_eventPool.remove(eventName);
    }
}

bool QEventForwarder::subscribe(QObject *listener, const QByteArray &eventName)
{
    if (!listener || eventName.isEmpty()) {
        m_lastError = QStringLiteral("Invalid listener or event name");
        return false;
    }

    QWriteLocker locker(&m_lock);
    
    // 清理已失效的监听器
    if (m_eventPool.contains(eventName)) {
        auto &listeners = m_eventPool[eventName];
        for (auto it = listeners.begin(); it != listeners.end();) {
            if (!it->data()) {
                it = listeners.erase(it);
            } else if (it->data() == listener) {
                m_lastError = QStringLiteral("Listener already subscribed to this event");
                return false;
            } else {
                ++it;
            }
        }
    }

    // 添加新的监听器
    m_eventPool[eventName].append(QPointer<QObject>(listener));
    
    // 当监听器被销毁时自动取消订阅
    QObject::connect(listener, &QObject::destroyed, [eventName]() {
        unsubscribe(nullptr, eventName);
    });

    return true;
}

bool QEventForwarder::publish(const QByteArray &eventName,
                           Qt::ConnectionType connectionType,
                           QGenericArgument val0,
                           QGenericArgument val1,
                           QGenericArgument val2,
                           QGenericArgument val3,
                           QGenericArgument val4,
                           QGenericArgument val5,
                           QGenericArgument val6,
                           QGenericArgument val7,
                           QGenericArgument val8,
                           QGenericArgument val9)
{
    if (eventName.isEmpty()) {
        m_lastError = QStringLiteral("Event name cannot be empty");
        return false;
    }

    QReadLocker locker(&m_lock);
    if (!m_eventPool.contains(eventName) || m_eventPool[eventName].isEmpty()) {
        m_lastError = QStringLiteral("No subscribers for event: %1").arg(QString(eventName));
        return false;
    }

    auto methodName = formatMethodName(eventName);
    QStringList errors;
    auto listeners = m_eventPool[eventName]; // 创建副本
    locker.unlock(); // 释放读取锁定

    bool hasValidListener = false;
    for (const auto &listener : listeners) {
        if (!listener) {
            continue;
        }

        hasValidListener = true;
        auto ret = QMetaObject::invokeMethod(
            listener.data(), methodName, connectionType,
            val0, val1, val2, val3, val4,
            val5, val6, val7, val8, val9);

        if (!ret) {
            errors.append(QStringLiteral("%1:%2")
                .arg(listener->metaObject()->className(),
                     listener->objectName()));
        }
    }

    if (!hasValidListener) {
        m_lastError = QStringLiteral("No valid listeners for event: %1").arg(QString(eventName));
        return false;
    }

    if (errors.isEmpty()) {
        return true;
    }

    m_lastError = QStringLiteral("Failed to deliver event %1 to:\n%2")
        .arg(QString(eventName), errors.join("\n"));
    return false;
}

} // namespace Core
