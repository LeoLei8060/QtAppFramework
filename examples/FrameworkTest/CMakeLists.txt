cmake_minimum_required(VERSION 3.14)

project(FrameworkTest LANGUAGES CXX)

# 源文件
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

# 框架源文件
set(FRAMEWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/event/EventManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/event/QEventForwarder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/utils/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/utils/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/utils/SingleApplication.cpp
    ${CMAKE_SOURCE_DIR}/src/gui/style/ThemeManager.cpp
    ${CMAKE_SOURCE_DIR}/src/network/NetworkManager.cpp
)

set(FRAMEWORK_HEADERS
    ${CMAKE_SOURCE_DIR}/src/core/event/EventManager.h
    ${CMAKE_SOURCE_DIR}/src/core/event/QEventForwarder.h
    ${CMAKE_SOURCE_DIR}/src/core/event/IEventHandler.h
    ${CMAKE_SOURCE_DIR}/src/core/utils/ConfigManager.h
    ${CMAKE_SOURCE_DIR}/src/core/utils/IPlugin.h
    ${CMAKE_SOURCE_DIR}/src/core/utils/PluginManager.h
    ${CMAKE_SOURCE_DIR}/src/core/utils/SingleApplication.h
    ${CMAKE_SOURCE_DIR}/src/gui/style/ThemeManager.h
    ${CMAKE_SOURCE_DIR}/src/network/NetworkManager.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${FRAMEWORK_SOURCES}
    ${FRAMEWORK_HEADERS}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 设置目标属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Network
)

# 复制资源文件到构建目录
set(RESOURCE_DIRS
    themes
    config
    plugins
)

foreach(DIR ${RESOURCE_DIRS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/${DIR}
    )
endforeach()

# 安装目标
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装资源文件
foreach(DIR ${RESOURCE_DIRS})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endforeach()

# 添加插件子目录
add_subdirectory(plugins/TestPlugin)
