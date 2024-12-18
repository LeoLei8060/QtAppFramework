#pragma once

#include "3rdparty/nameof/nameof.hpp"
#include <memory>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QObject>
#include <QReadWriteLock>
#include <QPointer>

#define EVENT_METHOD_PREFIX "On_Event_"

namespace Core {

/**
 * @brief 事件转发器类
 * 提供基于Qt元对象系统的事件发布订阅机制
 */
class QEventForwarder : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 取消订阅事件
     * @param listener 监听器对象
     * @param eventName 事件名称
     */
    static void unsubscribe(QObject *listener, const QByteArray &eventName);

    /**
     * @brief 订阅事件
     * @param listener 监听器对象
     * @param eventName 事件名称
     * @return 是否订阅成功
     */
    static bool subscribe(QObject *listener, const QByteArray &eventName);

    /**
     * @brief 发布事件（带连接类型）
     * @param eventName 事件名称
     * @param connectionType 连接类型
     * @param val0-val9 事件参数
     * @return 是否发布成功
     */
    static bool publish(const QByteArray  &eventName,
                      Qt::ConnectionType connectionType,
                      QGenericArgument   val0 = QGenericArgument(),
                      QGenericArgument   val1 = QGenericArgument(),
                      QGenericArgument   val2 = QGenericArgument(),
                      QGenericArgument   val3 = QGenericArgument(),
                      QGenericArgument   val4 = QGenericArgument(),
                      QGenericArgument   val5 = QGenericArgument(),
                      QGenericArgument   val6 = QGenericArgument(),
                      QGenericArgument   val7 = QGenericArgument(),
                      QGenericArgument   val8 = QGenericArgument(),
                      QGenericArgument   val9 = QGenericArgument());

    /**
     * @brief 发布事件（自动连接类型）
     */
    static inline bool publish(const QByteArray &eventName,
                           QGenericArgument  val0 = QGenericArgument(),
                           QGenericArgument  val1 = QGenericArgument(),
                           QGenericArgument  val2 = QGenericArgument(),
                           QGenericArgument  val3 = QGenericArgument(),
                           QGenericArgument  val4 = QGenericArgument(),
                           QGenericArgument  val5 = QGenericArgument(),
                           QGenericArgument  val6 = QGenericArgument(),
                           QGenericArgument  val7 = QGenericArgument(),
                           QGenericArgument  val8 = QGenericArgument(),
                           QGenericArgument  val9 = QGenericArgument())
    {
        return publish(eventName,
                     Qt::AutoConnection,
                     val0, val1, val2, val3, val4,
                     val5, val6, val7, val8, val9);
    }

    /**
     * @brief 获取最近的错误信息
     */
    static inline QString getLastError() { return m_lastError; }

    /**
     * @brief 清除所有事件订阅
     */
    static inline void clear()
    {
        QWriteLocker locker(&m_lock);
        m_eventPool.clear();
        m_lastError.clear();
    }

    /**
     * @brief 检查事件是否有订阅者
     */
    static inline bool hasSubscribers(const QByteArray &eventName)
    {
        QReadLocker locker(&m_lock);
        return m_eventPool.contains(eventName) && !m_eventPool[eventName].isEmpty();
    }

protected:
    /**
     * @brief 格式化事件方法名
     */
    static inline QByteArray formatMethodName(const QByteArray &eventName)
    {
        return EVENT_METHOD_PREFIX + eventName;
    }

private:
    static QMap<QByteArray, QList<QPointer<QObject>>> m_eventPool;
    static QReadWriteLock m_lock;
    static QString m_lastError;
};

/**
 * @brief 参数转换模板函数
 * @note 使用该函数时，槽函数参数类型必须使用原始类型名称，而不是typedef的别名
 */
template<typename T>
QGenericArgument toArg(T &&val)
{
    return QGenericArgument(NAMEOF_TYPE_EXPR(val).data(), &val);
}

} // namespace Core
