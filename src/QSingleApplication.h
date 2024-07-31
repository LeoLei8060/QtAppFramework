#ifndef __QSINGLEAPPLICATION_H__
#define __QSINGLEAPPLICATION_H__

#include <QApplication>
#include <QDataStream>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

class QSingleApplication : public QApplication
{
    Q_OBJECT

public:
    QSingleApplication(int &argc, char **argv, const QString &uniqueKey);

    bool isAnotherInstanceRunning() const { return isRunning; }

    bool sendMessage(const QString &message)
    {
        if (!isRunning)
            return false;

        QLocalSocket socket;
        socket.connectToServer(uniqueKey);
        if (!socket.waitForConnected(500)) {
            return false;
        }

        QByteArray  byteArray;
        QDataStream out(&byteArray, QIODevice::WriteOnly);
        out << message;

        socket.write(byteArray);
        socket.waitForBytesWritten();
        socket.disconnectFromServer();

        return true;
    }

signals:
    void messageReceived(const QString &message);

private slots:
    void handleNewConnection()
    {
        QLocalSocket *socket = localServer->nextPendingConnection();
        connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
            QDataStream in(socket);
            QString     message;
            in >> message;
            emit messageReceived(message);
            socket->disconnectFromServer();
        });
    }

private:
    QString       uniqueKey;
    QSharedMemory sharedMemory;
    QLocalServer *localServer = nullptr;
    bool          isRunning;
};

#endif // __QSINGLEAPPLICATION_H__
