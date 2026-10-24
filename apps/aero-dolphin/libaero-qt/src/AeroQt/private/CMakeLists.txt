# Yes it's an ugly botch, it's going soon, i promise

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)

# 1. Create an internal object target for the settings
add_library(AeroQtPrivateSettings OBJECT)

# 2. Attach the generated KConfig files to this local target
kconfig_add_kcfg_files(AeroQtPrivateSettings GENERATE_MOC aerothemesettings_gen.kcfgc)

# 3. Expose the root of the build tree so <AeroQt/private/...> resolves perfectly
target_include_directories(AeroQtPrivateSettings
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/src>
    PRIVATE
        KF6::ConfigGui
)

set_target_properties(AeroQtPrivateSettings PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(AeroQtPrivateSettings
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core    # <--- ADD THIS LINE to fix qglobal.h missing
        KF6::ConfigGui                 # Keeps kconfigskeleton.h satisfied
)

# 4. Handle its installation cleanly right here
install(FILES $<FILTER:$<TARGET_PROPERTY:AeroQtPrivateSettings,SOURCES>,INCLUDE,.*\.h$>
    DESTINATION include/AeroQt/private
)

# 4. Install the global KConfig XT schema file. This lets the system know what defaults your application uses globally
install(FILES aerothemesettings.kcfg
    DESTINATION ${KDE_INSTALL_KCFGDIR}
)
