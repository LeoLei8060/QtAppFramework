#pragma once

#include <QObject>
#include <QVariant>

namespace Core {

/**
 * @brief 事件处理器接口类
 * 所有需要处理事件的类都应该继承此接口
 */
class IEventHandler {
public:
    virtual ~IEventHandler() = default;

    /**
     * @brief 处理事件的虚函数
     * @param eventName 事件名称
     * @param data 事件数据
     * @return 是否成功处理事件
     */
    virtual bool handleEvent(const QString& eventName, const QVariant& data) = 0;

    /**
     * @brief 获取处理器优先级
     * @return 优先级值，数值越大优先级越高
     */
    virtual int priority() const { return 0; }
};

} // namespace Core
