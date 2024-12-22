set(TARGET1_NAME TrafficLightPlugin)

# 添加插件库
add_library(${TARGET1_NAME} SHARED
    TrafficLightPlugin.h
    TrafficLightPlugin.cpp
    TrafficLightWidget.h
    TrafficLightWidget.cpp
)

# 设置包含目录
target_include_directories(${TARGET1_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

# 链接 Qt 库
target_link_libraries(${TARGET1_NAME}
    PRIVATE
        QtConfig
        PluginCore
)

# 设置输出目录为插件目录
set_target_properties(${TARGET1_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)
