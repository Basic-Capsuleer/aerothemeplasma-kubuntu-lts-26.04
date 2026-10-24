#ifndef TASKMANAGERPLUGIN_H
#define TASKMANAGERPLUGIN_H

#include <QQmlExtensionPlugin>

class TaskManagerPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri) override;
};

#endif
