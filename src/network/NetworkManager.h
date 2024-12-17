#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <functional>

namespace Core {

/**
 * @brief 网络请求管理器
 * 提供以下功能：
 * 1. RESTful API 支持
 * 2. 请求队列和并发控制
 * 3. 请求重试机制
 * 4. 请求拦截器
 * 5. 响应缓存
 */
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    // 请求拦截器函数类型
    using RequestInterceptor = std::function<void(QNetworkRequest&)>;
    // 响应拦截器函数类型
    using ResponseInterceptor = std::function<void(QNetworkReply*)>;

    static NetworkManager* instance();

    /**
     * @brief 初始化网络管理器
     * @param maxConcurrent 最大并发请求数
     */
    void initialize(int maxConcurrent = 6);

    /**
     * @brief 添加请求拦截器
     */
    void addRequestInterceptor(RequestInterceptor interceptor);

    /**
     * @brief 添加响应拦截器
     */
    void addResponseInterceptor(ResponseInterceptor interceptor);

    /**
     * @brief 发送GET请求
     * @param url 请求URL
     * @param headers 请求头
     */
    QNetworkReply* get(const QString& url, const QMap<QString, QString>& headers = {});

    /**
     * @brief 发送POST请求
     * @param url 请求URL
     * @param data 请求数据
     * @param headers 请求头
     */
    QNetworkReply* post(const QString& url, 
                       const QJsonDocument& data,
                       const QMap<QString, QString>& headers = {});

    /**
     * @brief 发送PUT请求
     */
    QNetworkReply* put(const QString& url,
                      const QJsonDocument& data,
                      const QMap<QString, QString>& headers = {});

    /**
     * @brief 发送DELETE请求
     */
    QNetworkReply* deleteResource(const QString& url,
                                const QMap<QString, QString>& headers = {});

    /**
     * @brief 设置默认请求头
     */
    void setDefaultHeaders(const QMap<QString, QString>& headers);

    /**
     * @brief 设置请求超时时间
     */
    void setTimeout(int msecs);

    /**
     * @brief 设置重试次数
     */
    void setRetryCount(int count);

private:
    NetworkManager();
    ~NetworkManager() override;

    static NetworkManager* instance_;
    QNetworkAccessManager* manager_;
    QMap<QString, QString> defaultHeaders_;
    QList<RequestInterceptor> requestInterceptors_;
    QList<ResponseInterceptor> responseInterceptors_;
    int timeout_;
    int retryCount_;
    int maxConcurrent_;
    
    QNetworkRequest createRequest(const QString& url, const QMap<QString, QString>& headers);
    void applyInterceptors(QNetworkRequest& request);
    void handleResponse(QNetworkReply* reply);
};

} // namespace Core
