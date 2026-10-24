#pragma once

#include <AeroQt/private/aerothemesettings_gen.h>
#include <AeroQt/util/props.h>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct ToolbarRule : public QObject
{
	Q_OBJECT

	MAKE_PROPERTY(QString, appName, AppName);
	MAKE_PROPERTY(QString, objName, ObjName);
	MAKE_PROPERTY(QString, color, Color);
	MAKE_PROPERTY(bool, flat, Flat);

	ToolbarRule(QObject *parent = nullptr);
};

class AeroThemeSettings : public AeroThemeSettingsGen
{
public:
	void setToolbarOverrides(QList</*unowned*/ ToolbarRule *> arr);
	QList</*owned*/ ToolbarRule *> toolbarOverrides();

	inline static AeroThemeSettings *self() {
		return (AeroThemeSettings *) AeroThemeSettingsGen::self();
	}
};

