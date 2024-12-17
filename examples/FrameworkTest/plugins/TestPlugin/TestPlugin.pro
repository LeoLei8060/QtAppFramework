QT       += core gui

TARGET = TestPlugin
TEMPLATE = lib
CONFIG += plugin

INCLUDEPATH += ../../../src

DESTDIR = ../../plugins

SOURCES += \
    testplugin.cpp

HEADERS += \
    testplugin.h \
    ../../../src/core/utils/IPlugin.h

DISTFILES += TestPlugin.json
