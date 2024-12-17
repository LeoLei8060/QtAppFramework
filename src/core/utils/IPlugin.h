#pragma once

#include <QString>
#include <QVariant>

namespace Core {

/**
 * @brief 插件接口类
 * 所有插件必须实现此接口
 */
class IPlugin {
public:
    virtual ~IPlugin() = default;

    /**
     * @brief 获取插件名称
     */
    virtual QString name() const = 0;

    /**
     * @brief 获取插件版本
     */
    virtual QString version() const = 0;

    /**
     * @brief 获取插件描述
     */
    virtual QString description() const = 0;

    /**
     * @brief 初始化插件
     */
    virtual bool initialize() = 0;

    /**
     * @brief 卸载插件
     */
    virtual void unload() = 0;

    /**
     * @brief 插件是否已加载
     */
    virtual bool isLoaded() const = 0;

    /**
     * @brief 获取插件依赖
     */
    virtual QStringList dependencies() const = 0;

    /**
     * @brief 执行插件功能
     * @param command 命令名称
     * @param params 参数
     */
    virtual QVariant execute(const QString& command, const QVariantMap& params = {}) = 0;
};

} // namespace Core

#define PLUGIN_IID "com.qappframework.plugin"
Q_DECLARE_INTERFACE(Core::IPlugin, PLUGIN_IID)
