#pragma once

#include <KCModule>

#include <QJsonDocument>
#include <QJsonArray>
#include <AeroQt/util/props.h>

namespace Ui {
class SettingsPage;
}

struct ToolbarRule;

class AeroThemeKCM : public KCModule
{
	Q_OBJECT

	MAKE_PROPERTY(QJsonArray, tbRules, TbRules);

public:
	AeroThemeKCM(QObject *parent, const KPluginMetaData &data);
	virtual ~AeroThemeKCM();

	virtual void save() override;

private:
	void addTbRule(ToolbarRule *rule);

	Ui::SettingsPage *ui;
};
