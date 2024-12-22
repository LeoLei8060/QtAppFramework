add_library(PluginCore STATIC
    IPlugin.h
    PluginManager.h
    PluginManager.cpp
)

target_include_directories(PluginCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(PluginCore
    PUBLIC
        QtConfig
)
