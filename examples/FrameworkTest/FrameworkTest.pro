QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TEMPLATE = app

# 包含框架源代码
INCLUDEPATH += ../../src

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    ../../src/core/event/EventManager.cpp \
    ../../src/core/event/QEventForwarder.cpp \
    ../../src/core/utils/ConfigManager.cpp \
    ../../src/core/utils/PluginManager.cpp \
    ../../src/gui/style/ThemeManager.cpp \
    ../../src/network/NetworkManager.cpp

HEADERS += \
    mainwindow.h \
    ../../src/core/event/EventManager.h \
    ../../src/core/event/QEventForwarder.h \
    ../../src/core/event/IEventHandler.h \
    ../../src/core/utils/ConfigManager.h \
    ../../src/core/utils/IPlugin.h \
    ../../src/core/utils/PluginManager.h \
    ../../src/gui/style/ThemeManager.h \
    ../../src/network/NetworkManager.h

FORMS += \
    mainwindow.ui

# 默认规则使生成的文件输出到构建目录
target.path = $$[QT_INSTALL_EXAMPLES]/FrameworkTest
INSTALLS += target

# 复制资源文件到构建目录
CONFIG += file_copies
COPIES += themes configs plugins
themes.files = $$files(themes/*)
themes.path = $$OUT_PWD/themes
configs.files = $$files(config/*)
configs.path = $$OUT_PWD/config
plugins.files = $$files(plugins/*)
plugins.path = $$OUT_PWD/plugins
