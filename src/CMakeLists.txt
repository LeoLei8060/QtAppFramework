find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)

file(GLOB SRC_SOURCES "*.cpp" "*.h")

add_library(SRC SHARED ${SRC_SOURCES})

target_link_libraries(SRC PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
#target_sources(SRC INTERFACE ${SRC_SOURCES})
