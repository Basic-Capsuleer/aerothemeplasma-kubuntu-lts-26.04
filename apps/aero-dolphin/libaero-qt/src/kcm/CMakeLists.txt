# Test using `kcmshell6 kcm_aerotheme`

cmake_minimum_required(VERSION 3.16)
project(AeroThemeKCM)

find_package(KF6Config REQUIRED)
find_package(KF6KCMUtils REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 1. Load Extra CMake Modules to get standard installation directories
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

# Build KCM Plugin target
add_library(kcm_aerotheme MODULE
    aerothemekcm.cpp
    settingspage.ui
    toolbarruleeditor.cpp
    brandingoverrideseditor.cpp
    qvariantstuff.cpp
    ${gen_sources}
)

target_link_libraries(kcm_aerotheme
    PRIVATE
        KF6::KCMUtils
        KF6::ConfigCore
        Qt6::Widgets
        AeroQt
)

kcmutils_generate_desktop_file(kcm_aerotheme)

# 3. Install the KCM plugin to the standard KDE KCMs directory (e.g., /usr/lib/qt6/plugins/kcms)
install(TARGETS kcm_aerotheme
    DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms
)
