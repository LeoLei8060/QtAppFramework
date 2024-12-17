#include "NetworkManager.h"
#include <QTimer>
#include <QDebug>

namespace Core {

NetworkManager* NetworkManager::instance_ = nullptr;

NetworkManager* NetworkManager::instance()
{
    if (!instance_) {
        instance_ = new NetworkManager();
    }
    return instance_;
}

NetworkManager::NetworkManager()
    : QObject(nullptr)
    , manager_(new QNetworkAccessManager(this))
    , timeout_(30000)  // 默认30秒超时
    , retryCount_(3)   // 默认重试3次
    , maxConcurrent_(6)
{
    // 设置一些默认请求头
    defaultHeaders_["Content-Type"] = "application/json";
    defaultHeaders_["Accept"] = "application/json";
}

NetworkManager::~NetworkManager()
{
    delete manager_;
}

void NetworkManager::initialize(int maxConcurrent)
{
    maxConcurrent_ = maxConcurrent;
}

void NetworkManager::addRequestInterceptor(RequestInterceptor interceptor)
{
    requestInterceptors_.append(interceptor);
}

void NetworkManager::addResponseInterceptor(ResponseInterceptor interceptor)
{
    responseInterceptors_.append(interceptor);
}

QNetworkReply* NetworkManager::get(const QString& url, const QMap<QString, QString>& headers)
{
    QNetworkRequest request = createRequest(url, headers);
    QNetworkReply* reply = manager_->get(request);
    handleResponse(reply);
    return reply;
}

QNetworkReply* NetworkManager::post(const QString& url, 
                                  const QJsonDocument& data,
                                  const QMap<QString, QString>& headers)
{
    QNetworkRequest request = createRequest(url, headers);
    QNetworkReply* reply = manager_->post(request, data.toJson());
    handleResponse(reply);
    return reply;
}

QNetworkReply* NetworkManager::put(const QString& url,
                                 const QJsonDocument& data,
                                 const QMap<QString, QString>& headers)
{
    QNetworkRequest request = createRequest(url, headers);
    QNetworkReply* reply = manager_->put(request, data.toJson());
    handleResponse(reply);
    return reply;
}

QNetworkReply* NetworkManager::deleteResource(const QString& url,
                                           const QMap<QString, QString>& headers)
{
    QNetworkRequest request = createRequest(url, headers);
    QNetworkReply* reply = manager_->deleteResource(request);
    handleResponse(reply);
    return reply;
}

void NetworkManager::setDefaultHeaders(const QMap<QString, QString>& headers)
{
    defaultHeaders_ = headers;
}

void NetworkManager::setTimeout(int msecs)
{
    timeout_ = msecs;
}

void NetworkManager::setRetryCount(int count)
{
    retryCount_ = count;
}

QNetworkRequest NetworkManager::createRequest(const QString& url, 
                                           const QMap<QString, QString>& headers)
{
    QNetworkRequest request(url);
    
    // 应用默认请求头
    for (auto it = defaultHeaders_.begin(); it != defaultHeaders_.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
    
    // 应用自定义请求头
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    // 设置超时
    request.setTransferTimeout(timeout_);
    
    // 应用请求拦截器
    applyInterceptors(request);
    
    return request;
}

void NetworkManager::applyInterceptors(QNetworkRequest& request)
{
    for (const auto& interceptor : requestInterceptors_) {
        interceptor(request);
    }
}

void NetworkManager::handleResponse(QNetworkReply* reply)
{
    // 处理超时
    QTimer::singleShot(timeout_, reply, [reply]() {
        if (reply->isRunning()) {
            reply->abort();
        }
    });

    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        // 应用响应拦截器
        for (const auto& interceptor : responseInterceptors_) {
            interceptor(reply);
        }

        // 处理错误
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Network error:" << reply->errorString();
            
            // 如果需要重试
            static QMap<QNetworkReply*, int> retryCount;
            if (retryCount[reply] < retryCount_) {
                retryCount[reply]++;
                // 重新发送请求
                QNetworkRequest request = reply->request();
                QNetworkAccessManager::Operation operation = reply->operation();
                QByteArray data = reply->property("requestData").toByteArray();
                
                reply->deleteLater();
                QNetworkReply* newReply = nullptr;
                
                switch (operation) {
                    case QNetworkAccessManager::GetOperation:
                        newReply = get(request.url().toString());
                        break;
                    case QNetworkAccessManager::PostOperation:
                        newReply = post(request.url().toString(), QJsonDocument::fromJson(data));
                        break;
                    case QNetworkAccessManager::PutOperation:
                        newReply = put(request.url().toString(), QJsonDocument::fromJson(data));
                        break;
                    case QNetworkAccessManager::DeleteOperation:
                        newReply = deleteResource(request.url().toString());
                        break;
                    default:
                        break;
                }
                
                if (newReply) {
                    newReply->setProperty("requestData", data);
                }
                return;
            }
        }

        reply->deleteLater();
    });
}

} // namespace Core
