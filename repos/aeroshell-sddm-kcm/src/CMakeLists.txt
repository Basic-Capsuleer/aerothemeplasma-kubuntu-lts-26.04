include_directories(
    /usr/include/KF6/KAuth/
    /usr/include/KF6/KAuthCore/
    /usr/include/KF6/KConfigCore/
    /usr/include/KF6/KConfig/
)

add_subdirectory(helper)

set(kcm_SRCS
    kcm.cpp
    themesmodel.cpp
)

kcmutils_add_qml_kcm(kcm_aeroshellsddm SOURCES ${kcm_SRCS})
target_link_libraries(kcm_aeroshellsddm
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::I18n
    KF6::AuthCore
)

install(FILES aeroshell.desktop DESTINATION ${KDE_INSTALL_DATADIR}/systemsettings/categories)
