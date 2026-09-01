cmake_minimum_required(VERSION 3.16)

project(SmileMenu
    VERSION 4.4
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include(GNUInstallDirs)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    Network
)

find_package(LayerShellQt REQUIRED)

qt_standard_project_setup(REQUIRES 6.5)
qt_policy(SET QTP0004 NEW)

qt_add_executable(smilemenu
    smilemenu/main.cpp
    smilemenu/daemon.cpp
    smilemenu/daemon.h
    smilemenu/model.cpp
    smilemenu/model.h
    smilemenu/appitem.cpp
    smilemenu/appitem.h
    smilemenu/iconprovider.cpp
    smilemenu/iconprovider.h
    smilemenu/desktopentry.cpp
    smilemenu/desktopentry.h
    smilemenu/desktopcache.cpp
    smilemenu/desktopcache.h
    smilemenu/config.cpp
    smilemenu/config.h
    smilemenu/history.cpp
    smilemenu/history.h
    smilemenu/lock.cpp
    smilemenu/lock.h
    smilemenu/execparser.cpp
    smilemenu/execparser.h
    smilemenu/providers.cpp
    smilemenu/providers.h
    smilemenu/launcher.cpp
    smilemenu/launcher.h
)

target_compile_definitions(smilemenu
    PRIVATE
        SMILEMENU_VERSION="${PROJECT_VERSION}"
)

target_include_directories(smilemenu
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/smilemenu
)

target_link_libraries(smilemenu
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::Network
        LayerShellQt::Interface
)

qt6_add_qml_module(smilemenu
    URI SmileMenu
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

install(
    TARGETS smilemenu
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
