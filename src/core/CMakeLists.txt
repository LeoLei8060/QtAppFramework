# 添加事件相关源文件
add_library(QAppFrameworkCore INTERFACE)

target_include_directories(QAppFrameworkCore
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# 链接 Qt 库
target_link_libraries(QAppFrameworkCore
    INTERFACE
    QtConfig
)

# 添加插件子目录
add_subdirectory(plugin)
