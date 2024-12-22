# 示例项目配置
option(BUILD_FRAMEWORK_TEST "Build FrameworkTest example" ON)
option(BUILD_ZMQ_TEST "Build ZMQ test example" ON)
option(BUILD_EXAMPLE_PLUGIN "Build example plugin" ON)
option(BUILD_TRAFFIC_LIGHT_PLUGIN "Build traffic light plugin" ON)

if(BUILD_FRAMEWORK_TEST)
    add_subdirectory(FrameworkTest)
    message(STATUS "Building FrameworkTest example")
else()
    message(STATUS "Skipping FrameworkTest example")
endif()

if(BUILD_ZMQ_TEST)
    add_subdirectory(ZmqTest)
    message(STATUS "Building ZMQ test example")
else()
    message(STATUS "Skipping ZMQ test example")
endif()

if(BUILD_EXAMPLE_PLUGIN)
    add_subdirectory(ExamplePlugin)
    message(STATUS "Building example plugin")
else()
    message(STATUS "Skipping example plugin")
endif()


