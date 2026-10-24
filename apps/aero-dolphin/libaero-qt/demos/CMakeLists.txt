project(Demos LANGUAGES CXX)
cmake_minimum_required(VERSION 3.31)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(KF6WidgetsAddons REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6Completion REQUIRED)
find_package(KF6KIO REQUIRED)


add_subdirectory(flatpak-manager/)
