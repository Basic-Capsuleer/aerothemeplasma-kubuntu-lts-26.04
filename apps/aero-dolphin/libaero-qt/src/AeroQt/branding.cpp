#include "branding.h"

#include "private/aerothemesettings.h"
#include <QtCore>

using namespace Aero;

int Branding::reportedBuild()
{
	return AeroThemeSettings::self()->build();
}

QString overridesFile =
	QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
	"/aerothemeoverrides.dat";

QVariant Branding::custom(const char *key)
{
	// We don't load the whole thing so as to save memory. We don't want a 1MB QPixmap loaded in memory for every app that uses libAeroQt.
	static QVariantMap cache;

	if (cache.contains(key))
		return cache[key];

	/* Read */
	if (QFile::exists(overridesFile)) {
		QFile file(overridesFile);
		file.open(QIODevice::ReadOnly);

		QDataStream in(&file);

		while (!in.atEnd()) {
			QString name;
			QVariant val;

			in >> name >> val;

			if (name == key) {
				cache[key] = val;
				return val;
			}
		}
	}

	return QVariant();
}
