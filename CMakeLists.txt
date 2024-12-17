cmake_minimum_required(VERSION 3.14)

project(QAppFramework VERSION 1.0.0 LANGUAGES CXX)

# 设置 CMake 策略
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 启用 Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 允许用户选择 Qt 版本
option(USE_QT6 "Use Qt 6" ON)

# 查找 Qt 包
if(USE_QT6)
    find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Network
        QUIET
    )
endif()

if(NOT Qt6_FOUND)
    set(USE_QT6 OFF)
    find_package(Qt5 5.15 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED
    )
    set(Qt Qt5)
else()
    set(Qt Qt6)
endif()

message(STATUS "Using Qt ${Qt}")

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4 /MP)
else()
    add_compile_options(-Wall -Wextra)
endif()

# 设置安装目录
include(GNUInstallDirs)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

# 添加子目录
add_subdirectory(examples/FrameworkTest)
