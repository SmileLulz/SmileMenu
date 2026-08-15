cmake_minimum_required(VERSION 3.16)
project(SmileMenu VERSION 4.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)
find_package(LayerShellQt REQUIRED)

add_executable(smilemenu
    src/main.cpp
    src/daemon.cpp src/daemon.h
    src/model.cpp src/model.h
    src/appitem.cpp src/appitem.h
    src/iconprovider.cpp src/iconprovider.h
    src/desktopentry.cpp src/desktopentry.h
    src/desktopcache.cpp src/desktopcache.h
    src/config.cpp src/config.h
    src/history.cpp src/history.h
    src/lock.cpp src/lock.h
    src/execparser.cpp src/execparser.h
    src/providers.cpp src/providers.h
    src/launcher.cpp src/launcher.h
)

target_include_directories(smilemenu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(smilemenu PRIVATE
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
    QML_FILES qml/Main.qml
)

install(TARGETS smilemenu RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
