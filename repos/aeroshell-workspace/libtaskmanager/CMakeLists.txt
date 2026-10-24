set(taskmanagerplugin_SRCS
    taskmanagerplugin.h taskmanagerplugin.cpp

    backend.h backend.cpp

    smartlauncherbackend.h smartlauncherbackend.cpp
    smartlauncheritem.h smartlauncheritem.cpp
)

add_library(taskmanagerplugin SHARED ${taskmanagerplugin_SRCS})

kconfig_add_kcfg_files(taskmanagerplugin kactivitymanagerd_plugins_settings.kcfgc)

ecm_qt_declare_logging_category(taskmanagerplugin
    HEADER taskmanager-debug.h
    IDENTIFIER TASKMANAGER_DEBUG
    CATEGORY_NAME aeroshell.taskmanager
)

target_link_libraries(taskmanagerplugin
    PRIVATE
        Qt6::Core
        Qt6::Qml
        Qt6::Quick
        Plasma::Activities
        Plasma::ActivitiesStats
        KF6::ConfigGui
        KF6::I18n
        KF6::KIOCore
        KF6::KIOGui
        KF6::KIOFileWidgets
        KF6::Notifications
        Plasma::Plasma
        KSysGuard::ProcessCore
        KF6::Service
        KF6::WindowSystem
        PW::LibNotificationManager
)


install(TARGETS taskmanagerplugin DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/taskmanager)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/aeroshell/taskmanager)
