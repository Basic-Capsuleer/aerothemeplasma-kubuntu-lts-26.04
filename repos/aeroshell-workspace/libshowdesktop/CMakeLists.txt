set(showdesktopplugin_SRCS
    showdesktopplugin.h showdesktopplugin.cpp
    showdesktop.h showdesktop.cpp
)

qt_add_dbus_interface(showdesktopplugin_SRCS org.kde.KWin.xml kwin_interface)

add_library(showdesktopplugin SHARED ${showdesktopplugin_SRCS})

target_link_libraries(showdesktopplugin
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
        KF6::WindowSystem
)

install(TARGETS showdesktopplugin DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/showdesktop)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/showdesktop)

