#include "backend.h"

#include "smartlauncheritem.h"

#include "taskmanagerplugin.h"

#include <QQmlEngine>

void TaskManagerPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<Backend>(uri, 1, 0, "Backend");
    qmlRegisterType<SmartLauncher::Item>(uri, 1, 0, "SmartLauncherItem");
}

#include "moc_taskmanagerplugin.cpp"
