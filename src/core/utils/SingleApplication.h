#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>

namespace Core {

class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(int &argc, char **argv, const QString &appKey);
    ~SingleApplication() override;

    bool isRunning();
    bool sendMessage(const QString &message);

signals:
    void messageReceived(const QString &message);

private slots:
    void handleConnection();
    void receiveMessage();

private:
    bool initializeSharedMemory();
    void cleanupSharedMemory();

    QString appKey_;
    QSharedMemory sharedMemory_;
    QLocalServer *localServer_;
    static const int timeout_ = 1000;
};

} // namespace Core

#endif // SINGLEAPPLICATION_H
