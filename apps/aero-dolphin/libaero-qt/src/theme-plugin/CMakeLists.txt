# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

cmake_minimum_required(VERSION 3.16)
project(AeroQt-Kvantum VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(KF6WidgetsAddons)
find_package(KF6WindowSystem)

find_package(ECM 6.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)

# Define the modular plugin library target
add_library(AeroQt-Kvantum MODULE)

target_sources(AeroQt-Kvantum PRIVATE
        customqstyleplugin.cpp
        customqstyleplugin.h
)

# 3. FIX: Link dependencies (Removed nonexistent Qt6::ProxyStyle)
target_link_libraries(AeroQt-Kvantum
        PUBLIC
        Qt6::Core
        Qt6::Widgets
        AeroQt
)

set_target_properties(AeroQt-Kvantum PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Installation macros
install(TARGETS AeroQt-Kvantum DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/styles/)
install(FILES LibAeroQt-Kvantum.themerc DESTINATION ${KDE_INSTALL_DATADIR}/kstyle/themes)
