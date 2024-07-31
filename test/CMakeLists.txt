project(${PROJECTNAME})

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

set(SOURCES
    main.cpp
)

add_executable(${PROJECTNAME} ${SOURCES})

target_link_libraries(${PROJECTNAME} PRIVATE Qt5::Core Qt5::Widgets Qt5::Network SRC)
