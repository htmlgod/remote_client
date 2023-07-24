cmake_minimum_required(VERSION 3.16)
project(rd_client VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Network Widgets X11Extras)

add_executable(rd_client WIN32 MACOSX_BUNDLE
    common.h
    key_mapper.cpp key_mapper.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    xlib_utils.h
)
target_link_libraries(rd_client PRIVATE
    Qt5::Core
    Qt5::Gui
    X11
    Xfixes
    Xtst
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(rd_client PRIVATE
        Qt5::Network
        Qt5::Widgets
        Qt5::X11Extras
    )
endif()

install(TARGETS rd_client
    RUNTIME DESTINATION /usr/local/remote_client
)
include(InstallRequiredSystemLibraries)
set(CPACK_DEBIAN_PACKAGE_NAME "remote-client")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "CNIIAG")
include(CPack)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
