#include "ConfigManager.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>

namespace Core {

ConfigManager* ConfigManager::instance_ = nullptr;

ConfigManager* ConfigManager::instance()
{
    if (!instance_) {
        instance_ = new ConfigManager();
    }
    return instance_;
}

ConfigManager::ConfigManager()
    : QObject(nullptr)
{
}

ConfigManager::~ConfigManager()
{
    // 保存所有配置
    saveConfig(App);
    saveConfig(User);
    for (auto it = moduleConfigs_.begin(); it != moduleConfigs_.end(); ++it) {
        saveJsonToFile(moduleConfigPaths_[it.key()], it.value());
    }
}

bool ConfigManager::initialize(const QString& appConfigPath, const QString& userConfigPath)
{
    QWriteLocker locker(&lock_);
    appConfigPath_ = appConfigPath;
    userConfigPath_ = userConfigPath;

    // 确保配置文件目录存在
    QDir().mkpath(QFileInfo(appConfigPath_).path());
    QDir().mkpath(QFileInfo(userConfigPath_).path());

    // 加载配置文件
    if (!loadJsonFromFile(appConfigPath_, appConfig_)) {
        appConfig_ = QJsonObject();
    }
    if (!loadJsonFromFile(userConfigPath_, userConfig_)) {
        userConfig_ = QJsonObject();
    }

    return true;
}

QVariant ConfigManager::getValue(const QString& key, const QVariant& defaultValue, ConfigLevel level) const
{
    QReadLocker locker(&lock_);
    QVariant value;

    switch (level) {
        case App:
            value = getValueFromJson(appConfig_, key);
            break;
        case User:
            value = getValueFromJson(userConfig_, key);
            break;
        case Module: {
            // 从key中提取模块名（假设格式为"moduleName.key"）
            QString moduleName = key.split('.').first();
            if (moduleConfigs_.contains(moduleName)) {
                value = getValueFromJson(moduleConfigs_[moduleName], key.mid(moduleName.length() + 1));
            }
            break;
        }
    }

    return value.isValid() ? value : defaultValue;
}

bool ConfigManager::setValue(const QString& key, const QVariant& value, ConfigLevel level)
{
    QWriteLocker locker(&lock_);
    bool success = false;

    switch (level) {
        case App:
            success = setValueInJson(appConfig_, key, value);
            break;
        case User:
            success = setValueInJson(userConfig_, key, value);
            break;
        case Module: {
            QString moduleName = key.split('.').first();
            if (moduleConfigs_.contains(moduleName)) {
                success = setValueInJson(moduleConfigs_[moduleName], 
                                      key.mid(moduleName.length() + 1), 
                                      value);
            }
            break;
        }
    }

    if (success) {
        emit configChanged(key, value, level);
    }
    return success;
}

bool ConfigManager::registerModuleConfig(const QString& moduleName, const QString& configPath)
{
    QWriteLocker locker(&lock_);
    
    if (moduleConfigs_.contains(moduleName)) {
        return false;
    }

    QJsonObject moduleConfig;
    if (!loadJsonFromFile(configPath, moduleConfig)) {
        moduleConfig = QJsonObject();
    }

    moduleConfigPaths_[moduleName] = configPath;
    moduleConfigs_[moduleName] = moduleConfig;
    return true;
}

bool ConfigManager::saveConfig(ConfigLevel level)
{
    QReadLocker locker(&lock_);
    switch (level) {
        case App:
            return saveJsonToFile(appConfigPath_, appConfig_);
        case User:
            return saveJsonToFile(userConfigPath_, userConfig_);
        case Module:
            for (auto it = moduleConfigs_.begin(); it != moduleConfigs_.end(); ++it) {
                if (!saveJsonToFile(moduleConfigPaths_[it.key()], it.value())) {
                    return false;
                }
            }
            return true;
    }
    return false;
}

bool ConfigManager::reloadConfig(ConfigLevel level)
{
    QWriteLocker locker(&lock_);
    switch (level) {
        case App:
            return loadJsonFromFile(appConfigPath_, appConfig_);
        case User:
            return loadJsonFromFile(userConfigPath_, userConfig_);
        case Module:
            for (auto it = moduleConfigPaths_.begin(); it != moduleConfigPaths_.end(); ++it) {
                QJsonObject newConfig;
                if (!loadJsonFromFile(it.value(), newConfig)) {
                    return false;
                }
                moduleConfigs_[it.key()] = newConfig;
            }
            return true;
    }
    return false;
}

QVariant ConfigManager::getValueFromJson(const QJsonObject& json, const QString& key) const
{
    QStringList parts = key.split('.');
    QJsonObject current = json;
    
    // 遍历多级键
    for (int i = 0; i < parts.size() - 1; ++i) {
        if (!current.contains(parts[i]) || !current[parts[i]].isObject()) {
            return QVariant();
        }
        current = current[parts[i]].toObject();
    }

    // 获取最终值
    QString lastKey = parts.last();
    if (!current.contains(lastKey)) {
        return QVariant();
    }
    return current[lastKey].toVariant();
}

bool ConfigManager::setValueInJson(QJsonObject& json, const QString& key, const QVariant& value)
{
    QStringList parts = key.split('.');
    if (parts.isEmpty()) {
        return false;
    }

    if (parts.size() == 1) {
        // 单层键，直接设置值
        json[parts[0]] = QJsonValue::fromVariant(value);
        return true;
    }

    // 处理多层键，使用递归方式
    QString currentKey = parts[0];
    QJsonObject obj = json[currentKey].toObject();
    
    // 构建剩余的键
    QStringList remainingParts = parts.mid(1);
    QString remainingKey = remainingParts.join('.');
    
    // 递归设置剩余部分
    if (setValueInJson(obj, remainingKey, value)) {
        json[currentKey] = obj;
        return true;
    }
    
    return false;
}

bool ConfigManager::loadJsonFromFile(const QString& path, QJsonObject& json)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        return false;
    }

    json = doc.object();
    return true;
}

bool ConfigManager::saveJsonToFile(const QString& path, const QJsonObject& json)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

} // namespace Core
