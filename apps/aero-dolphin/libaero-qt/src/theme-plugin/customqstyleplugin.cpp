#include "customqstyleplugin.h"

#include "../AeroQt/private/customqstyle.h"

CustomQStylePlugin::CustomQStylePlugin(QObject *parent)
	: QStylePlugin(parent)
{
}

QStyle *CustomQStylePlugin::create(const QString &key)
{
	if (key.compare(QLatin1String("libaeroqt-kvantum"), Qt::CaseInsensitive) == 0) {
        return new Aero::Private::CustomQStyle;
	}
	return nullptr;
}

QStringList CustomQStylePlugin::keys() const
{
	return QStringList(QStringLiteral("LibAeroQt-Kvantum"));
}

