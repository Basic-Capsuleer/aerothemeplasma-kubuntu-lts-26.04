set(aeroshellutilsplugin_SRCS
    aeroshellutilsplugin.h aeroshellutilsplugin.cpp
    sddm.h sddm.cpp
)

add_library(aeroshellutilsplugin SHARED ${aeroshellutilsplugin_SRCS})

target_link_libraries(aeroshellutilsplugin
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
        KF6::ConfigCore
)

install(TARGETS aeroshellutilsplugin DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/utils)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/utils)

