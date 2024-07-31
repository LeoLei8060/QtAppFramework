#include "QSingleApplication.h"

QSingleApplication::QSingleApplication(int &argc, char **argv, const QString &uniqueKey)
    : QApplication(argc, argv)
    , uniqueKey(uniqueKey)
    , isRunning(false)
{
    sharedMemory.setKey(uniqueKey);

    // 尝试创建共享内存段，如果失败则表示已有实例在运行
    if (sharedMemory.create(1)) {
        // 没有其他实例在运行
        localServer = new QLocalServer(this);
        connect(localServer,
                &QLocalServer::newConnection,
                this,
                &QSingleApplication::handleNewConnection);

        QString serverName = uniqueKey;
        QFile::remove(serverName); // 删除之前的残留文件
        if (!localServer->listen(serverName)) {
            qWarning() << "Unable to start the local server:" << localServer->errorString();
        }
    } else {
        // 另一个实例正在运行
        isRunning = true;
    }
}
