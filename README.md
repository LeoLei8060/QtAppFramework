# QAppFramework

QAppFramework 是一个基于 Qt 的现代化应用程序框架，提供了一套完整的开发工具和模块化组件，旨在简化桌面应用程序的开发过程。

## 特性

- 模块化设计
- 主题管理系统
- 网络请求框架
- 事件管理系统
- 配置管理
- 插件系统

## 目录结构

```
QAppFramework/
├── src/                    # 源代码
│   ├── core/              # 核心功能模块
│   │   ├── event/        # 事件系统
│   │   └── utils/        # 工具类
│   ├── gui/              # GUI 相关模块
│   │   └── style/       # 主题和样式
│   └── network/          # 网络模块
├── examples/              # 示例项目
│   └── FrameworkTest/    # 框架测试程序
└── docs/                 # 文档
```

## 模块说明

### 1. 事件管理系统 (EventManager)

事件管理系统提供了一个灵活的事件发布/订阅机制，支持异步事件处理和优先级控制。

**设计思想：**
- 解耦组件间的通信
- 支持异步和同步事件处理
- 基于优先级的事件处理机制

**使用示例：**
```cpp
// 注册事件处理器
class MyHandler : public IEventHandler {
public:
    bool handleEvent(const QString& eventName, const QVariant& data) override {
        if (eventName == "TestEvent") {
            // 处理事件
            return true;
        }
        return false;
    }
};

// 发送事件
EventManager::instance()->postEvent("TestEvent", "Hello World");
```

### 2. 配置管理器 (ConfigManager)

配置管理器提供了一个分层的配置系统，支持应用级、用户级和模块级配置。

**设计思想：**
- 分层配置管理
- JSON 格式存储
- 支持配置热重载
- 配置变更通知机制

**使用示例：**
```cpp
// 初始化配置
ConfigManager::instance()->initialize("app.json", "user.json");

// 读取配置
QString theme = ConfigManager::instance()->getValue("app.theme", "default").toString();

// 修改配置
ConfigManager::instance()->setValue("user.language", "zh_CN", ConfigManager::User);
```

### 3. 主题管理器 (ThemeManager)

主题管理器提供了动态主题切换和自定义主题支持。

**设计思想：**
- 主题的动态加载和切换
- JSON 格式的主题定义
- 支持主题继承和覆盖
- 实时预览功能

**使用示例：**
```cpp
// 初始化主题管理器
ThemeManager::instance()->initialize("themes");

// 切换主题
ThemeManager::instance()->setTheme("dark");

// 创建自定义主题
ThemeManager::instance()->createCustomTheme("custom", "default");
```

### 4. 网络管理器 (NetworkManager)

网络管理器提供了一个高级的 HTTP 请求框架。

**设计思想：**
- 请求队列管理
- 拦截器机制
- 错误重试
- 并发控制

**使用示例：**
```cpp
// 发送 GET 请求
auto reply = NetworkManager::instance()->get("https://api.example.com/data");
connect(reply, &QNetworkReply::finished, this, &MyClass::onRequestFinished);

// 添加请求拦截器
NetworkManager::instance()->addRequestInterceptor([](QNetworkRequest& request) {
    request.setHeader(QNetworkRequest::UserAgentHeader, "MyApp/1.0");
});
```

### 5. 插件系统 (PluginManager)

插件系统支持动态加载和管理插件。

**设计思想：**
- 动态加载/卸载
- 插件依赖管理
- 版本控制
- 热插拔支持

**使用示例：**
```cpp
// 加载插件
PluginManager::instance()->loadPlugin("TestPlugin");

// 获取插件实例
auto plugin = PluginManager::instance()->getPlugin("TestPlugin");
```

### 6. 单例应用程序 (SingleApplication)

提供应用程序单例模式支持，防止多个实例同时运行。

**设计思想：**
- 使用共享内存确保单例
- 进程间通信支持
- 消息传递机制

**使用示例：**
```cpp
// 创建单例应用程序
int main(int argc, char *argv[])
{
    Core::SingleApplication app(argc, argv, "MyUniqueAppKey");
    
    if (app.isRunning()) {
        // 发送消息给已运行的实例
        app.sendMessage("show");
        return 0;
    }
    
    // 处理来自其他实例的消息
    QObject::connect(&app, &SingleApplication::messageReceived,
        [](const QString &message) {
            if (message == "show") {
                // 激活主窗口
                mainWindow->show();
                mainWindow->raise();
                mainWindow->activateWindow();
            }
        });
    
    // 继续应用程序初始化...
    return app.exec();
}
```

## 功能模块状态

| 模块名称 | 状态 | 说明 | 计划发布版本 |
|---------|------|------|-------------|
| 事件管理系统 | 已完成 | 支持同步/异步事件、优先级控制 | v1.0 |
| 配置管理器 | 已完成 | 分层配置、热重载 | v1.0 |
| 主题管理器 | 已完成 | 动态主题切换、自定义主题 | v1.0 |
| 网络管理器 | 已完成 | HTTP 请求框架、拦截器 | v1.0 |
| 插件系统 | 已完成 | 插件加载、依赖管理 | v1.0 |
| 日志系统 | 开发中 | 分级日志、文件轮转 | v1.1 |
| 数据库管理器 | 计划中 | ORM 支持、连接池 | v1.2 |
| 缓存系统 | 计划中 | 内存/文件缓存、过期策略 | v1.2 |
| 任务调度器 | 计划中 | 定时任务、并发控制 | v1.3 |
| 国际化支持 | 计划中 | 多语言、动态切换 | v1.3 |
| 窗口管理器 | 计划中 | MDI/SDI 支持、窗口状态保存 | v1.4 |
| 更新管理器 | 计划中 | 自动更新、增量更新 | v1.4 |

## 构建要求

- Qt 5.15+ 或 Qt 6.x
- CMake 3.14+
- C++17 兼容的编译器

## 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 贡献指南

欢迎提交 Issue 和 Pull Request。在提交 PR 之前，请确保：

1. 代码符合项目的编码规范
2. 添加了适当的单元测试
3. 更新了相关文档
4. 所有测试用例都能通过
