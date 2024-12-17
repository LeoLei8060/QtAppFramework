#pragma once

#include "QEventForwarder.h"
#include "IEventHandler.h"
#include <QObject>
#include <QMap>
#include <QReadWriteLock>
#include <memory>

namespace Core {

/**
 * @brief 事件管理器类
 * 提供更高级的事件管理功能，包括：
 * 1. 事件优先级管理
 * 2. 事件过滤
 * 3. 事件队列
 * 4. 异步事件处理
 */
class EventManager : public QObject
{
    Q_OBJECT
public:
    static EventManager* instance();

    /**
     * @brief 注册事件处理器
     * @param handler 处理器指针
     * @param eventNames 要处理的事件名称列表
     */
    void registerHandler(IEventHandler* handler, const QStringList& eventNames);

    /**
     * @brief 注销事件处理器
     * @param handler 处理器指针
     */
    void unregisterHandler(IEventHandler* handler);

    /**
     * @brief 发送事件
     * @param eventName 事件名称
     * @param data 事件数据
     * @param async 是否异步处理
     * @return 是否成功发送
     */
    bool postEvent(const QString& eventName, const QVariant& data = QVariant(), bool async = false);

    /**
     * @brief 通过QEventForwarder发送事件
     * @param eventName 事件名称
     * @param connectionType 连接类型
     * @param args 参数列表
     * @return 是否成功发送
     */
    template<typename... Args>
    bool postEventWithArgs(const QString& eventName, Qt::ConnectionType connectionType, Args&&... args) {
        return QEventForwarder::publish(eventName.toLatin1(), connectionType, 
            toArg(std::forward<Args>(args))...);
    }

    template<typename... Args>
    bool postEventWithArgs(const QString& eventName, Args&&... args) {
        return postEventWithArgs(eventName, Qt::AutoConnection, std::forward<Args>(args)...);
    }

    /**
     * @brief 检查事件是否有处理器
     * @param eventName 事件名称
     * @return 是否有处理器
     */
    bool hasHandler(const QString& eventName) const;

private:
    EventManager();
    ~EventManager() override;

    static EventManager* instance_;
    QMap<QString, QList<IEventHandler*>> handlers_;
    QReadWriteLock lock_;

    void sortHandlers(const QString& eventName);
};

} // namespace Core
