cmake_minimum_required(VERSION 3.5)

project(QAppFramework VERSION 1.0.0 LANGUAGES CXX)

# 项目选项
option(BUILD_EXAMPLES "Build example projects" ON)
option(BUILD_TESTS "Build test projects" OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找 Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Network)

message(STATUS "Using Qt ${QT_VERSION_MAJOR}")

# 创建一个接口库来传递 Qt 设置
add_library(QtConfig INTERFACE)
target_link_libraries(QtConfig INTERFACE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# 添加第三方库
add_subdirectory(3rdparty)

# 添加源代码目录
add_subdirectory(src)

# 根据选项添加示例项目
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
    message(STATUS "Building example projects")
else()
    message(STATUS "Skipping example projects")
endif()

# 根据选项添加测试项目
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Building test projects")
else()
    message(STATUS "Skipping test projects")
endif()
