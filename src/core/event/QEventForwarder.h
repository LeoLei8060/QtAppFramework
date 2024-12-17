#pragma once

//#include "3rdparty/nameof/nameof.hpp"
//#include "GlobalShared/MessageMacros.h"

#include <memory>
#include <QDebug>
#include <QList>
#include <QMap>
#include <QObject>
#include <QReadWriteLock>

#define METHOD_PREFIX "On_Event_"

namespace Core {
class QEventForwarder : public QObject
{
    Q_OBJECT
public:
    static void unSubscribe(QObject *listener, const QByteArray &eventName);

    static bool subscribe(QObject *listener, const QByteArray &eventName);

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
                       val0,
                       val1,
                       val2,
                       val3,
                       val4,
                       val5,
                       val6,
                       val7,
                       val8,
                       val9);
    }

    static inline QString get_Errors() { return ps_LastError_; }

    static inline void clearEvents()
    {
        QWriteLocker locker(&ps_Lock_);
        psEvents_pool_.clear();
    }

    static inline QByteArray methodFormatting(const QByteArray &eventName)
    {
        return METHOD_PREFIX + eventName;
    }

private:
    static QMap<QByteArray, QList<QObject *>> psEvents_pool_;

    static QReadWriteLock ps_Lock_;

    static QString ps_LastError_;
};

/*
* 使用该模板函数需要注意：
*   T类型如果使用了typedef来对变量类型进行重新命名，那么在槽函数声明时必须写原类型名称，例如：
*   typedef QString Text;   那么槽函数声明时的参数类型必须写QString，如果写的是Text，那么槽函数将无法被调用
*/

template<typename T>
QGenericArgument toArg(T &&val)
{
    //    return QGenericArgument(NAMEOF_TYPE_EXPR(val).data(), &val);
    return QGenericArgument(typeid(val).name(), &val);
}
} // namespace Core
