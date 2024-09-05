cmake_minimum_required(VERSION 3.5)
set(PROJECTNAME QtAppFrameworkTest)
project(${PROJECTNAME})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置构建类型，如果没有指定，则默认为 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# 检测平台和架构
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCH_DIR x64)
else()
    set(ARCH_DIR x86)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_DIR windows)
else()
    set(PLATFORM_DIR unix)
endif()

# 指定生成文件的输出路径，区分不同架构和构建类型
set(OUTPUT_BASE_DIR ${CMAKE_SOURCE_DIR}/bin)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/${CMAKE_BUILD_TYPE}/${PLATFORM_DIR}/${ARCH_DIR})

# 设置动态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/${CMAKE_BUILD_TYPE}/${PLATFORM_DIR}/${ARCH_DIR})

# 设置静态库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_BASE_DIR}/${CMAKE_BUILD_TYPE}/${PLATFORM_DIR}/${ARCH_DIR})


set(FONTFILE_PATH ${CMAKE_SOURCE_DIR}/test/res/font)
set(FONTFILE_SAVEPATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/font)
# 拷贝
if(EXISTS "${FONTFILE_PATH}")
    execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${FONTFILE_PATH} ${FONTFILE_SAVEPATH})
else()
    message(WARNING "copy failed")
endif()


#add_subdirectory(src)
add_subdirectory(test)
