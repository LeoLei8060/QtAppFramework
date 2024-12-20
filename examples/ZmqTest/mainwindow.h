#pragma once

#include <memory>
#include <QMainWindow>
#include <QThread>
#include <zmq.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// 发布者线程
class PublisherThread : public QThread
{
    Q_OBJECT
public:
    explicit PublisherThread(QObject *parent = nullptr);
    ~PublisherThread();

    void stop();
    void *getSocket() const { return m_socket; }

signals:
    void finished();

protected:
    void run() override;

private:
    void *m_context;
    void *m_socket;
    bool m_running;
};

// 订阅者线程
class SubscriberThread : public QThread
{
    Q_OBJECT
public:
    explicit SubscriberThread(const QString &filter, QObject *parent = nullptr);
    ~SubscriberThread();

    void stop();
    void *getSocket() const { return m_socket; }

signals:
    void messageReceived(const QString &message);
    void finished();

protected:
    void run() override;

private:
    void *m_context;
    void *m_socket;
    bool m_running;
    QString m_filter;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void finished();

private slots:
    void onStartPublisher();
    void onStopPublisher();
    void onStartSubscriber();
    void onStopSubscriber();
    void onPublishMessage();
    void onMessageReceived(const QString &message);

private:
    void setupUi();
    void setupConnections();
    void appendLog(const QString &message);

private:
    Ui::MainWindow *ui;
    
    // 发布者线程
    std::unique_ptr<PublisherThread> m_publisherThread;
    bool m_publisherRunning;
    
    // 订阅者线程
    std::unique_ptr<SubscriberThread> m_subscriberThread;
    bool m_subscriberRunning;
};
