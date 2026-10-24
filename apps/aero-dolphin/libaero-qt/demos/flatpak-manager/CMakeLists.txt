cmake_minimum_required(VERSION 3.20)
project(aerothemeplasma-extras)

# 1. Find Extra CMake Modules (ECM) which provides find-modules for KF6
find_package(ECM 6.0.0 REQUIRED NO_MODULE)

# 2. Append ECM's module directories into your CMake search paths
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

# 3. Include standard KDE macros (highly recommended for include/binary setups)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)   # Allow char* -> QString

# find_package(AeroQt REQUIRED)

find_package(KF6 REQUIRED COMPONENTS XmlGui)

qt_add_executable(FlatpakManager
  mainwindow.cpp mainwindow.h mainwindow.ui
  installwizard_qt.h installwizard_qt.cpp installwizard_qt.ui
)

target_link_libraries(FlatpakManager
  PRIVATE
    KF6::CoreAddons
    KF6::Completion
    KF6::KIOWidgets
    KF6::KIOFileWidgets
    KF6::XmlGui
    AeroQt
)
