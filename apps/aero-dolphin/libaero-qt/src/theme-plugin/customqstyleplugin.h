#pragma once

#include <QStylePlugin>

class CustomQStylePlugin : public QStylePlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QStyleFactoryInterface" FILE "LibAeroQt-Kvantum.json")

public:
	explicit CustomQStylePlugin(QObject *parent = nullptr);

	QStringList keys() const;

	QStyle *create(const QString &) override;
};
