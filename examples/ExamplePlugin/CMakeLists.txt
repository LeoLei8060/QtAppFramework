set(TARGET_NAME ExamplePlugin)
set(APP_NAME ExamplePluginApp)

# 添加红绿灯插件子目录
add_subdirectory(TrafficLightPlugin)

# 添加 UI 文件
set(UI_FILES
    mainwindow.ui
)

# 添加插件库
add_library(${TARGET_NAME} SHARED
    ExamplePlugin.h
    ExamplePlugin.cpp
)

# 设置包含目录
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

# 链接 Qt 库
target_link_libraries(${TARGET_NAME}
    PRIVATE
        QtConfig
        PluginCore
)

# 设置输出目录为插件目录
set_target_properties(${TARGET_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)

# 添加可执行程序
add_executable(${APP_NAME}
    main.cpp
    mainwindow.h
    mainwindow.cpp
    ${UI_FILES}
)

# 设置包含目录
target_include_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

# 链接 Qt 库和插件核心库
target_link_libraries(${APP_NAME}
    PRIVATE
        QtConfig
        PluginCore
)

# 设置输出目录
set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

