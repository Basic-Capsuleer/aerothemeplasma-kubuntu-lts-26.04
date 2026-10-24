cmake_minimum_required(VERSION 3.16.0)

project(aeroshellsddm-kcm)

set(PROJECT_VERSION "6.6.5")
set(QT_MAJOR_VERSION 6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 6.22.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQSTRING_DECL_DELETED_ASCII_OP)

find_package(Qt6 CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    Quick
)

find_package(KF6 REQUIRED COMPONENTS
    Config
    CoreAddons
    KCMUtils
    I18n
    Auth
)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
