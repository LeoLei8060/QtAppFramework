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

    bool initializeStyleSheet(const QString &styleSheetPath);

    bool initializeFonts(const QString &fontDirectory);

    bool isAnotherInstanceRunning() const { return m_isRunning; }

    bool sendMessage(const QString &message);

    static QSingleApplication *app() { return static_cast<QSingleApplication *> qApp; }

signals:
    void messageReceived(const QString &message);

private slots:
    void handleNewConnection();

private:
    QString       m_uniqueKey;
    QSharedMemory m_sharedMemory;
    QLocalServer *m_localServer = nullptr;
    bool          m_isRunning;
};

#endif // __QSINGLEAPPLICATION_H__
