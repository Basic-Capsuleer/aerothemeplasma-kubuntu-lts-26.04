#include "showdesktop.h"

#include "showdesktopplugin.h"

#include <QQmlEngine>

void ShowDesktopPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<ShowDesktop>(uri, 1, 0, "ShowDesktop");
}

#include "moc_showdesktopplugin.cpp"
