cmake_minimum_required(VERSION 3.14)

project(TestPlugin LANGUAGES CXX)

# 源文件
set(PROJECT_SOURCES
    testplugin.cpp
    testplugin.h
    TestPlugin.json
)

# 创建插件库
add_library(${PROJECT_NAME} MODULE
    ${PROJECT_SOURCES}
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# 设置目标属性
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${Qt}::Core
    ${Qt}::Gui
)

# 复制插件描述文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/TestPlugin.json
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/TestPlugin.json
)

# 安装插件
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/plugins
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins
)

# 安装插件描述文件
install(FILES TestPlugin.json
    DESTINATION ${CMAKE_INSTALL_BINDIR}/plugins
)
