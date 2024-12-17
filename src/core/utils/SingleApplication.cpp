#include "SingleApplication.h"
#include <QLocalSocket>
#include <QDataStream>

namespace Core {

SingleApplication::SingleApplication(int &argc, char **argv, const QString &appKey)
    : QApplication(argc, argv)
    , appKey_(appKey)
    , sharedMemory_(appKey)
    , localServer_(nullptr)
{
    // 尝试初始化共享内存
    initializeSharedMemory();
}

SingleApplication::~SingleApplication()
{
    cleanupSharedMemory();
    if (localServer_) {
        localServer_->close();
        delete localServer_;
    }
}

bool SingleApplication::initializeSharedMemory()
{
    // 如果已经存在共享内存，说明已经有实例在运行
    if (sharedMemory_.attach()) {
        return false;
    }

    // 创建共享内存
    if (!sharedMemory_.create(1)) {
        return false;
    }

    // 创建本地服务器用于进程间通信
    localServer_ = new QLocalServer(this);
    connect(localServer_, &QLocalServer::newConnection, this, &SingleApplication::handleConnection);

    // 确保之前的服务器实例被清理
    QLocalServer::removeServer(appKey_);
    
    // 启动服务器
    if (!localServer_->listen(appKey_)) {
        return false;
    }

    return true;
}

void SingleApplication::cleanupSharedMemory()
{
    if (sharedMemory_.isAttached()) {
        sharedMemory_.detach();
    }
}

bool SingleApplication::isRunning()
{
    return !localServer_;
}

bool SingleApplication::sendMessage(const QString &message)
{
    if (!isRunning()) {
        return false;
    }

    QLocalSocket socket;
    socket.connectToServer(appKey_, QIODevice::WriteOnly);

    if (!socket.waitForConnected(timeout_)) {
        return false;
    }

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << message;

    socket.write(buffer);
    if (!socket.waitForBytesWritten(timeout_)) {
        return false;
    }

    socket.disconnectFromServer();
    return true;
}

void SingleApplication::handleConnection()
{
    QLocalSocket *socket = localServer_->nextPendingConnection();
    if (!socket) {
        return;
    }

    connect(socket, &QLocalSocket::readyRead, this, &SingleApplication::receiveMessage);
    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}

void SingleApplication::receiveMessage()
{
    QLocalSocket *socket = qobject_cast<QLocalSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray buffer = socket->readAll();
    QDataStream stream(buffer);
    QString message;
    stream >> message;

    emit messageReceived(message);
}

} // namespace Core
