#include "EventManager.h"
#include <QCoreApplication>
#include <QtConcurrent/QtConcurrent>

namespace Core {

EventManager *EventManager::instance_ = nullptr;

EventManager *EventManager::instance()
{
    if (!instance_) {
        instance_ = new EventManager();
    }
    return instance_;
}

EventManager::EventManager()
    : QObject(nullptr)
{}

EventManager::~EventManager()
{
    QWriteLocker locker(&lock_);
    handlers_.clear();
}

void EventManager::registerHandler(IEventHandler *handler, const QStringList &eventNames)
{
    if (!handler)
        return;

    QWriteLocker locker(&lock_);
    for (const auto &eventName : eventNames) {
        if (!handlers_[eventName].contains(handler)) {
            handlers_[eventName].append(handler);
            sortHandlers(eventName);
        }
    }
}

void EventManager::unregisterHandler(IEventHandler *handler)
{
    if (!handler)
        return;

    QWriteLocker locker(&lock_);
    for (auto it = handlers_.begin(); it != handlers_.end(); ++it) {
        it.value().removeAll(handler);
    }
}

bool EventManager::postEvent(const QString &eventName, const QVariant &data, bool async)
{
    QReadLocker locker(&lock_);
    bool        hasHandlers = handlers_.contains(eventName) && !handlers_[eventName].isEmpty();
    auto handlerList = hasHandlers ? handlers_[eventName] : QList<IEventHandler *>(); // 创建副本
    locker.unlock();

    // 尝试使用 QEventForwarder
    bool forwarderResult = false;
    if (data.isValid()) {
        // 如果数据是 QVariantList，尝试将其作为参数列表传递给 QEventForwarder
        if (data.canConvert<QVariantList>()) {
            QVariantList            args = data.toList();
            QList<QGenericArgument> genericArgs;
            for (const QVariant &arg : args) {
                genericArgs.append(toArg(arg));
            }
            // 填充剩余的参数为空
            while (genericArgs.size() < 10) {
                genericArgs.append(QGenericArgument());
            }

            forwarderResult = QEventForwarder::publish(eventName.toLatin1(),
                                                       async ? Qt::QueuedConnection
                                                             : Qt::DirectConnection,
                                                       genericArgs[0],
                                                       genericArgs[1],
                                                       genericArgs[2],
                                                       genericArgs[3],
                                                       genericArgs[4],
                                                       genericArgs[5],
                                                       genericArgs[6],
                                                       genericArgs[7],
                                                       genericArgs[8],
                                                       genericArgs[9]);
        } else {
            // 单个参数的情况
            forwarderResult = QEventForwarder::publish(eventName.toLatin1(),
                                                       async ? Qt::QueuedConnection
                                                             : Qt::DirectConnection,
                                                       toArg(data));
        }
    }

    // 如果没有注册的处理器且 QEventForwarder 处理成功，直接返回
    if (!hasHandlers && forwarderResult) {
        return true;
    }

    // 如果有注册的处理器，继续使用 EventManager 的处理方式
    auto processEvent = [=]() {
        bool handled = false;
        for (auto *handler : handlerList) {
            if (handler && handler->handleEvent(eventName, data)) {
                handled = true;
                break;
            }
        }
        return handled;
    };

    if (async) {
        QtConcurrent::run(processEvent);
        return true;
    } else {
        return processEvent() || forwarderResult;
    }
}

bool EventManager::hasHandler(const QString &eventName) const
{
    QReadLocker locker(const_cast<QReadWriteLock*>(&lock_));
    return handlers_.contains(eventName) && !handlers_[eventName].isEmpty();
}

void EventManager::sortHandlers(const QString &eventName)
{
    if (!handlers_.contains(eventName))
        return;

    std::sort(handlers_[eventName].begin(),
              handlers_[eventName].end(),
              [](IEventHandler *a, IEventHandler *b) { return a->priority() > b->priority(); });
}

} // namespace Core
