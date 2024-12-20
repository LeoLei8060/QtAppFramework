# ZMQ 配置
if(MINGW AND Qt6_FOUND)
    # 在 MinGW + Qt6 环境下禁用 IPC 检查
    set(ZMQ_HAVE_IPC OFF CACHE BOOL "Disable IPC check for MinGW" FORCE)
    set(SKIP_IPC_CHECK ON CACHE BOOL "Skip IPC check" FORCE)
endif()

# 禁用 ZMQ 的一些不需要的功能
set(ENABLE_CURVE OFF CACHE BOOL "Enable CURVE security" FORCE)
set(WITH_DOCS OFF CACHE BOOL "Build Reference Guide documentation(requires DocBook)" FORCE)
set(WITH_DOC OFF CACHE BOOL "Build Reference Guide documentation(requires DocBook)" FORCE)
set(WITH_PERF_TOOL OFF CACHE BOOL "Build performance tools" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "Disable CPack" FORCE)
set(WITH_TLS OFF CACHE BOOL "Build with TLS support" FORCE)
set(WITH_LIBSODIUM OFF CACHE BOOL "Build with libsodium" FORCE)
set(BUILD_STATIC OFF CACHE BOOL "Build with static lib" FORCE)

# 添加 ZMQ 子模块
add_subdirectory(libzmq)

# 创建一个接口库来传递 ZMQ 设置
add_library(ZMQConfig INTERFACE)
target_include_directories(ZMQConfig INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libzmq/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(ZMQConfig INTERFACE libzmq)
