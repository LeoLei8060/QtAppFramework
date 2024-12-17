#include "QEventForwarder.h"
#include <QWriteLocker>

namespace Core {
QMap<QByteArray, QList<QObject *>> QEventForwarder::psEvents_pool_;
QReadWriteLock                     QEventForwarder::ps_Lock_;
QString                            QEventForwarder::ps_LastError_;

void QEventForwarder::unSubscribe(QObject *listener, const QByteArray &eventName)
{
    QWriteLocker locker(&ps_Lock_);
    int          index = -1;
    if (psEvents_pool_.contains(eventName) && (index = psEvents_pool_[eventName].indexOf(listener)) >= 0
        && index < psEvents_pool_[eventName].count())
        psEvents_pool_[eventName].takeAt(index);
}

bool QEventForwarder::subscribe(QObject *listener, const QByteArray &eventName)
{
    QWriteLocker locker(&ps_Lock_);
    if (psEvents_pool_.contains(eventName)) {
        if (-1 != psEvents_pool_[eventName].indexOf(listener)) {
            ps_LastError_ = QString("This object is subscribed to this eventName");
            return false;
        }
        psEvents_pool_[eventName].push_back(listener);
        return true;
    } else {
        psEvents_pool_.insert(eventName, {listener});
        return true;
    }
}

bool QEventForwarder::publish(const QByteArray  &eventName,
                              Qt::ConnectionType connectionType,
                              QGenericArgument   val0,
                              QGenericArgument   val1,
                              QGenericArgument   val2,
                              QGenericArgument   val3,
                              QGenericArgument   val4,
                              QGenericArgument   val5,
                              QGenericArgument   val6,
                              QGenericArgument   val7,
                              QGenericArgument   val8,
                              QGenericArgument   val9)
{
    QReadLocker locker(&ps_Lock_);
    if (!psEvents_pool_.contains(eventName)) {
        ps_LastError_ = QString("No objects subscribe to this eventName");
        return false;
    }
    auto        methodName = methodFormatting(eventName);
    QStringList errors;
    auto        listeners = psEvents_pool_[eventName]; // 创建副本
    locker.unlock();                                   // 释放读取锁定
    for (auto listener : listeners) {
        if (!listener)
            continue;
        auto ret = QMetaObject::invokeMethod(
            listener, methodName, connectionType, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
        if (!ret)
            errors.append(QString("%1:%2").arg(listener->metaObject()->className(), listener->objectName()));
    }
    if (errors.isEmpty())
        return true;
    ps_LastError_ = QString("%1 execution failed:[\n").arg(QString(eventName));
    for (auto &err : errors)
        ps_LastError_ += QString("%1;\n").arg(err);
    ps_LastError_ += "]\n";
    return false;
}
} // namespace Core
