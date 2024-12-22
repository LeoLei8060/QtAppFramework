# Qt Plugin System Example

这个示例展示了一个灵活的 Qt 插件系统的实现。该系统支持动态加载插件，并提供了一个统一的接口来处理不同类型的插件功能。

## 插件系统架构

### 核心组件

1. **IPlugin 基类**
   - 提供统一的插件接口
   - 支持多种功能接口：
     - UI 界面
     - 数据处理
     - 数据提供
     - 数据接收
     - 命令执行

2. **PluginManager**
   - 管理插件的加载和卸载
   - 提供插件查询功能
   - 处理插件生命周期

### 插件功能接口

插件可以实现以下功能：

1. **UI 相关功能**
   ```cpp
   virtual bool hasWidget() const
   virtual QWidget* createWidget(QWidget* parent = nullptr)
   virtual WidgetPosition widgetPosition() const
   ```

2. **数据处理功能**
   ```cpp
   virtual bool canProcessData() const
   virtual QString processData(const QString& data)
   ```

3. **数据访问功能**
   ```cpp
   virtual bool canProvideData() const
   virtual QVariant getData(const QString& key)
   ```

4. **数据设置功能**
   ```cpp
   virtual bool canAcceptData() const
   virtual bool setData(const QString& key, const QVariant& value)
   ```

5. **命令执行功能**
   ```cpp
   virtual bool canExecute() const
   virtual bool execute(const QString& command, const QVariantMap& params)
   ```

## 示例插件

### 1. ExamplePlugin
- 实现了数据处理功能
- 可以计算文本长度
- 展示了基本的插件实现方式

### 2. TrafficLightPlugin
- 实现了 UI 功能
- 提供了一个交通信号灯控件
- 展示了如何创建和管理插件 UI

## 使用方法

1. **加载插件**
   ```cpp
   PluginManager::instance()->loadPlugin("path/to/plugin.dll");
   ```

2. **获取特定功能的插件**
   ```cpp
   // 获取 UI 插件
   auto uiPlugins = PluginManager::instance()->getUIPlugins();
   
   // 获取数据处理插件
   auto processors = PluginManager::instance()->getDataProcessorPlugins();
   ```

3. **使用插件功能**
   ```cpp
   // 使用 UI 插件
   if (plugin->hasWidget()) {
       QWidget* widget = plugin->createWidget();
       // 添加到界面...
   }
   
   // 使用数据处理插件
   if (plugin->canProcessData()) {
       QString result = plugin->processData(data);
       // 处理结果...
   }
   ```

## 开发新插件

1. **创建新插件类**
   ```cpp
   class MyPlugin : public IPlugin {
       Q_OBJECT
       Q_PLUGIN_METADATA(IID PLUGIN_IID)
       Q_INTERFACES(IPlugin)
   public:
       // 实现必要的接口...
   };
   ```

2. **实现基本接口**
   ```cpp
   bool initialize() override;
   QString name() const override;
   QString version() const override;
   QString description() const override;
   QStringList supportedInterfaces() const override;
   ```

3. **实现所需功能**
   - 重写相应的虚函数
   - 实现具体的功能逻辑

