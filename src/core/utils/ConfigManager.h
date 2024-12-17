#pragma once

#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <QReadWriteLock>
#include <memory>

namespace Core {

/**
 * @brief 配置管理器类
 * 提供分层的配置管理功能，支持：
 * 1. JSON格式配置文件
 * 2. 多级配置（应用级、用户级、模块级）
 * 3. 配置热重载
 * 4. 配置变更通知
 */
class ConfigManager : public QObject
{
    Q_OBJECT
public:
    enum ConfigLevel {
        App,        // 应用级配置
        User,       // 用户级配置
        Module      // 模块级配置
    };

    static ConfigManager* instance();

    /**
     * @brief 初始化配置管理器
     * @param appConfigPath 应用配置文件路径
     * @param userConfigPath 用户配置文件路径
     */
    bool initialize(const QString& appConfigPath, const QString& userConfigPath);

    /**
     * @brief 获取配置值
     * @param key 配置键（支持点号分隔的多级键）
     * @param defaultValue 默认值
     * @param level 配置级别
     */
    QVariant getValue(const QString& key, 
                     const QVariant& defaultValue = QVariant(),
                     ConfigLevel level = App) const;

    /**
     * @brief 设置配置值
     * @param key 配置键
     * @param value 配置值
     * @param level 配置级别
     */
    bool setValue(const QString& key, 
                 const QVariant& value,
                 ConfigLevel level = App);

    /**
     * @brief 注册模块配置
     * @param moduleName 模块名称
     * @param configPath 配置文件路径
     */
    bool registerModuleConfig(const QString& moduleName, const QString& configPath);

    /**
     * @brief 保存配置到文件
     * @param level 配置级别
     */
    bool saveConfig(ConfigLevel level = App);

    /**
     * @brief 重新加载配置
     * @param level 配置级别
     */
    bool reloadConfig(ConfigLevel level = App);

signals:
    void configChanged(const QString& key, const QVariant& value, ConfigLevel level);

private:
    ConfigManager();
    ~ConfigManager() override;

    static ConfigManager* instance_;
    
    QString appConfigPath_;
    QString userConfigPath_;
    QMap<QString, QString> moduleConfigPaths_;
    
    QJsonObject appConfig_;
    QJsonObject userConfig_;
    QMap<QString, QJsonObject> moduleConfigs_;
    
    mutable QReadWriteLock lock_;

    QVariant getValueFromJson(const QJsonObject& json, const QString& key) const;
    bool setValueInJson(QJsonObject& json, const QString& key, const QVariant& value);
    bool loadJsonFromFile(const QString& path, QJsonObject& json);
    bool saveJsonToFile(const QString& path, const QJsonObject& json);
};

} // namespace Core
