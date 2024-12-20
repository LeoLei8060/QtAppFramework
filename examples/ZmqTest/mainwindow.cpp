#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <zmq.h>

// 发布者线程实现
PublisherThread::PublisherThread(QObject *parent)
    : QThread(parent)
    , m_context(nullptr)
    , m_socket(nullptr)
    , m_running(false)
{
}

PublisherThread::~PublisherThread()
{
    stop();
    wait();
}

void PublisherThread::stop()
{
    m_running = false;
}

void PublisherThread::run()
{
    m_context = zmq_ctx_new();
    m_socket = zmq_socket(m_context, ZMQ_PUB);
    
    // 绑定到本地端口
    if (zmq_bind(m_socket, "tcp://*:5555") != 0) {
        emit finished();
        return;
    }
    
    m_running = true;
    
    // 发布线程主循环
    while (m_running) {
        // 发布一些测试数据
        const char *data = "Test data";
        zmq_send(m_socket, data, strlen(data), 0);
        msleep(1000); // 每秒发送一次
    }
    
    // 清理资源
    zmq_close(m_socket);
    zmq_ctx_destroy(m_context);
}

// 订阅者线程实现
SubscriberThread::SubscriberThread(const QString &filter, QObject *parent)
    : QThread(parent)
    , m_context(nullptr)
    , m_socket(nullptr)
    , m_running(false)
    , m_filter(filter)
{
}

SubscriberThread::~SubscriberThread()
{
    stop();
    wait();
}

void SubscriberThread::stop()
{
    m_running = false;
}

void SubscriberThread::run()
{
    m_context = zmq_ctx_new();
    m_socket = zmq_socket(m_context, ZMQ_SUB);
    
    // 连接到发布者
    if (zmq_connect(m_socket, "tcp://localhost:5555") != 0) {
        emit finished();
        return;
    }
    
    // 设置订阅过滤器
    zmq_setsockopt(m_socket, ZMQ_SUBSCRIBE, m_filter.toUtf8().constData(), m_filter.length());
    
    m_running = true;
    
    // 订阅线程主循环
    while (m_running) {
        char buffer[1024];
        int size = zmq_recv(m_socket, buffer, sizeof(buffer) - 1, ZMQ_DONTWAIT);
        
        if (size > 0) {
            buffer[size] = '\0';
            emit messageReceived(QString::fromUtf8(buffer));
        }
        
        msleep(100); // 避免过度占用 CPU
    }
    
    // 清理资源
    zmq_close(m_socket);
    zmq_ctx_destroy(m_context);
}

// MainWindow 实现
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_publisherRunning(false)
    , m_subscriberRunning(false)
{
    ui->setupUi(this);
    setupUi();
    setupConnections();
}

MainWindow::~MainWindow()
{
    if (m_publisherRunning) {
        onStopPublisher();
    }
    if (m_subscriberRunning) {
        onStopSubscriber();
    }
    delete ui;
}

void MainWindow::setupUi()
{
    // 禁用需要发布者或订阅者运行的按钮
    ui->btnStopPublisher->setEnabled(false);
    ui->btnStopSubscriber->setEnabled(false);
    ui->btnPublish->setEnabled(false);
}

void MainWindow::setupConnections()
{
    // 发布者控制
    connect(ui->btnStartPublisher, &QPushButton::clicked, this, &MainWindow::onStartPublisher);
    connect(ui->btnStopPublisher, &QPushButton::clicked, this, &MainWindow::onStopPublisher);
    connect(ui->btnPublish, &QPushButton::clicked, this, &MainWindow::onPublishMessage);
    
    // 订阅者控制
    connect(ui->btnStartSubscriber, &QPushButton::clicked, this, &MainWindow::onStartSubscriber);
    connect(ui->btnStopSubscriber, &QPushButton::clicked, this, &MainWindow::onStopSubscriber);
}

void MainWindow::onStartPublisher()
{
    if (!m_publisherRunning) {
        m_publisherThread = std::make_unique<PublisherThread>(this);
        connect(m_publisherThread.get(), &PublisherThread::finished, this, [this]() {
            onStopPublisher();
        });
        
        m_publisherThread->start();
        m_publisherRunning = true;
        
        // 更新UI状态
        ui->btnStartPublisher->setEnabled(false);
        ui->btnStopPublisher->setEnabled(true);
        ui->btnPublish->setEnabled(true);
        ui->editPublisherEndpoint->setEnabled(false);
        
        appendLog("Publisher started");
    }
}

void MainWindow::onStopPublisher()
{
    if (m_publisherRunning) {
        m_publisherThread->stop();
        m_publisherThread->wait();
        m_publisherThread.reset();
        m_publisherRunning = false;
        
        // 更新UI状态
        ui->btnStartPublisher->setEnabled(true);
        ui->btnStopPublisher->setEnabled(false);
        ui->btnPublish->setEnabled(false);
        ui->editPublisherEndpoint->setEnabled(true);
        
        appendLog("Publisher stopped");
    }
}

void MainWindow::onStartSubscriber()
{
    if (!m_subscriberRunning) {
        const QString filter = ui->editFilter->text();
        m_subscriberThread = std::make_unique<SubscriberThread>(filter, this);
        
        connect(m_subscriberThread.get(), &SubscriberThread::messageReceived,
                this, &MainWindow::onMessageReceived);
        connect(m_subscriberThread.get(), &SubscriberThread::finished,
                this, [this]() {
            onStopSubscriber();
        });
        
        m_subscriberThread->start();
        m_subscriberRunning = true;
        
        // 更新UI状态
        ui->btnStartSubscriber->setEnabled(false);
        ui->btnStopSubscriber->setEnabled(true);
        ui->editSubscriberEndpoint->setEnabled(false);
        ui->editFilter->setEnabled(false);
        
        appendLog("Subscriber started with filter: " + filter);
    }
}

void MainWindow::onStopSubscriber()
{
    if (m_subscriberRunning) {
        m_subscriberThread->stop();
        m_subscriberThread->wait();
        m_subscriberThread.reset();
        m_subscriberRunning = false;
        
        // 更新UI状态
        ui->btnStartSubscriber->setEnabled(true);
        ui->btnStopSubscriber->setEnabled(false);
        ui->editSubscriberEndpoint->setEnabled(true);
        ui->editFilter->setEnabled(true);
        
        appendLog("Subscriber stopped");
    }
}

void MainWindow::onPublishMessage()
{
    if (!m_publisherRunning || !m_publisherThread) {
        return;
    }
    
    const QString message = ui->editMessage->text();
    if (message.isEmpty()) {
        return;
    }

    // 发布消息
    const QByteArray data = message.toUtf8();
    zmq_send(m_publisherThread->getSocket(), data.constData(), data.size(), 0);
    
    appendLog("Published: " + message);
    
    // 清空消息输入框
    ui->editMessage->clear();
}

void MainWindow::onMessageReceived(const QString &message)
{
    appendLog("Received: " + message);
}

void MainWindow::appendLog(const QString &message)
{
    const QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    ui->textLog->appendPlainText(QString("[%1] %2").arg(timestamp, message));
}
