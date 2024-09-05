#include "QSingleApplication.h"
#include <QDir>
#include <QFile>
#include <QFontDatabase>

QSingleApplication::QSingleApplication(int &argc, char **argv, const QString &uniqueKey)
    : QApplication(argc, argv)
    , m_uniqueKey(uniqueKey)
    , m_isRunning(false)
{
    m_sharedMemory.setKey(uniqueKey);

    // 尝试创建共享内存段，如果失败则表示已有实例在运行
    if (m_sharedMemory.create(1)) {
        // 没有其他实例在运行
        m_localServer = new QLocalServer(this);
        connect(m_localServer,
                &QLocalServer::newConnection,
                this,
                &QSingleApplication::handleNewConnection);

        QString serverName = uniqueKey;
        QFile::remove(serverName); // 删除之前的残留文件
        if (!m_localServer->listen(serverName)) {
            qWarning() << "Unable to start the local server:" << m_localServer->errorString();
        }
    } else {
        // 另一个实例正在运行
        m_isRunning = true;
    }
}

bool QSingleApplication::initializeStyleSheet(const QString &styleSheetPath)
{
    QFile file(styleSheetPath);
    if (file.open(QIODevice::ReadOnly)) {
        QString styleSheet = file.readAll();
        this->setStyleSheet(styleSheet);
        return true;
    } else {
        qWarning() << "Unable to open style sheet file:" << styleSheetPath;
        return false;
    }
}

bool QSingleApplication::initializeFonts(const QString &fontDirectory)
{
    QStringList fontFiles = QDir(fontDirectory).entryList(QDir::Files);
    for (const QString &fontFile : fontFiles) {
        int fontID = QFontDatabase::addApplicationFont(fontDirectory + "/" + fontFile);
        if (fontID < 0)
            qWarning() << "Failed to load font file: " << fontFile;
        else
            qDebug() << "load font ID: " << QFontDatabase::applicationFontFamilies(fontID).at(0);
    }
    return true;
}

bool QSingleApplication::sendMessage(const QString &message)
{
    if (!m_isRunning)
        return false;

    QLocalSocket socket;
    socket.connectToServer(m_uniqueKey);
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

void QSingleApplication::handleNewConnection()
{
    QLocalSocket *socket = m_localServer->nextPendingConnection();
    connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
        QDataStream in(socket);
        QString     message;
        in >> message;
        emit messageReceived(message);
        socket->disconnectFromServer();
    });
}
