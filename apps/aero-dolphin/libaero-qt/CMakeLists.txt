cmake_minimum_required(VERSION 3.16)

project(libAeroQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_VER 6.0.1)  # Major version follows Qt version

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets UiPlugin)
find_package(KF6WidgetsAddons REQUIRED)
find_package(KF6FrameworkIntegration REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6Completion REQUIRED)
find_package(KF6KIO REQUIRED)
find_package(KF6Config REQUIRED)


# Compile Library

file(GLOB_RECURSE sources
  "${CMAKE_CURRENT_LIST_DIR}/src/AeroQt/*.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/src/AeroQt/*.h"
  "${CMAKE_CURRENT_LIST_DIR}/src/AeroQt/*.ui"
)

qt_add_library(AeroQt SHARED
  ${sources}
  src/resources/res.qrc
)

# Generated settings structs. TODO: Move these into Borealis
add_subdirectory(src/AeroQt/private)

target_include_directories(AeroQt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>         # Natively handles <AeroQt/private/...>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src/AeroQt>  # Natively handles "private/..."
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(AeroQt
    PUBLIC
      Qt${QT_VERSION_MAJOR}::Core
      Qt${QT_VERSION_MAJOR}::Widgets
      Qt${QT_VERSION_MAJOR}::UiPlugin
      KF6::WidgetsAddons
      KF6::WindowSystem
      KF6::Style
      KF6::ConfigCore
      KF6::ConfigGui
      KF6::KIOFileWidgets
    PRIVATE
      AeroQtPrivateSettings
)
set_target_properties(AeroQt PROPERTIES
    VERSION ${LIB_VER}
    SOVERSION 0
)

# Install library binaries
install(TARGETS AeroQt
    EXPORT AeroQtTargets
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install headers
install(DIRECTORY src/AeroQt/
    DESTINATION include/AeroQt
    FILES_MATCHING PATTERN "*.h"
)

# Create CMake package & install

install(EXPORT AeroQtTargets
    FILE AeroQtTargets.cmake
    NAMESPACE AeroQt::
    DESTINATION lib/cmake/AeroQt
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AeroQtConfigVersion.cmake"
    VERSION 1.0
    COMPATIBILITY SameMajorVersion
)

file(WRITE
    "${CMAKE_CURRENT_BINARY_DIR}/AeroQtConfig.cmake"
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/AeroQtTargets.cmake\")"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/AeroQtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/AeroQtConfigVersion.cmake"
    DESTINATION lib/cmake/AeroQt
)

#######

add_subdirectory(demos/)
add_subdirectory(src/theme-plugin/)
add_subdirectory(src/kcm/)
