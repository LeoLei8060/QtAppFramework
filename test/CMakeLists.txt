project(${PROJECTNAME})

find_package(Qt5 COMPONENTS Core Widgets Network REQUIRED)
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

qt5_add_resources(QRC_FILES res.qrc)

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${CMAKE_SOURCE_DIR}/src/QSingleApplication.h
    ${CMAKE_SOURCE_DIR}/src/QSingleApplication.cpp
    ${QRC_FILES}
)


add_executable(${PROJECTNAME} ${SOURCES})

target_link_libraries(${PROJECTNAME} PRIVATE Qt5::Core Qt5::Widgets Qt5::Network)
