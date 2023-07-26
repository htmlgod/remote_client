cmake_minimum_required(VERSION 3.16)
project(remote-client VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Network Widgets X11Extras)

configure_file(common.h.in common.h)

add_executable(${PROJECT_NAME}
    main.cpp
    key_mapper.cpp
    mainwindow.cpp mainwindow.ui
)
target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    X11
    Xfixes
    Xtst
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Network
        Qt5::Widgets
        Qt5::X11Extras
    )
endif()


include(installing.cmake)
include(debpackaging.cmake)
