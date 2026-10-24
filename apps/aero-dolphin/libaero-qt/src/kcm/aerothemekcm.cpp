#include "aerothemekcm.h"

#include <KPluginFactory>
#include <QtWidgets>
#include <AeroQt/private/aerothemesettings.h>

#include "toolbarruleeditor.h"
#include "brandingoverrideseditor.h"

#include "ui_settingspage.h"

K_PLUGIN_FACTORY_WITH_JSON(MyKcmFactory, "kcm_aerotheme.json", registerPlugin<AeroThemeKCM>();)
// Icon: either preferences-desktop-plasma-theme or application-x-windows-themepack

auto *g_sett = AeroThemeSettings::self();

AeroThemeKCM::AeroThemeKCM(QObject *parent, const KPluginMetaData &data)
	: KCModule(parent, data)
	, ui(new Ui::SettingsPage)
{
	ui->setupUi(widget());
	addConfig(g_sett, widget());	// Register settings to manage automated saving when "Apply" is pressed

	auto continueButMarkChanged = [=](QVariant in) -> QVariant {
		this->markAsChanged();
		return in;
	};

	/* Branding */

	ui->buildBox->addItem("Windows Vista", 6000);
	ui->buildBox->addItem("Windows 7 Beta (7000)", 7000);
	ui->buildBox->addItem("Windows 7", 7600);
	ui->buildBox->addItem("Windows 8 Release Preview (8400)", 8400);

	bind_prop_bidi(
		g_sett, "build", &AeroThemeSettings::BuildChanged,
		ui->buildBox, "currentIndex", &QComboBox::currentIndexChanged,
		true,
		[=](QVariant build) { int idx = ui->buildBox->findData(build); return QVariant((idx == -1) ? 3 : idx); },
		[=](QVariant idx) { this->markAsChanged(); return ui->buildBox->itemData(idx.value<int>()); }
	);

	connect(ui->editOverrides, &QAbstractButton::clicked, [=]() {
		auto *t = new BrandingOverridesEditor(widget());
		t->open();
	});

	/* Tweaks */

	bind_prop_bidi(
		g_sett, "noFadeNavButtons", &AeroThemeSettings::NoFadeNavButtonsChanged,
		ui->noFadeNavButtons, "checked", &QAbstractButton::toggled,
		true, {}, continueButMarkChanged
	);

	/* Toolbar rules window */
	{
		connect(ui->tbList, &QListWidget::itemSelectionChanged, [=]() {
			ui->tbRemove->setEnabled(
				ui->tbList->selectedItems().length() > 0
			);
		});

		connect(ui->tbAdd, &QAbstractButton::clicked, [=]() {
			auto *rule = new ToolbarRule(this);
			addTbRule(rule);

			auto *t = new ToolbarRuleEditor(rule, widget());
			t->open();

			this->markAsChanged();
		});

		connect(ui->tbList, &QListWidget::itemActivated, [=](QListWidgetItem *item) {
			auto *t = new ToolbarRuleEditor(
				item->data(Qt::UserRole).value<ToolbarRule *>(),
				widget()
			);
			t->open();

			this->markAsChanged();
		});

		connect(ui->tbRemove, &QAbstractButton::clicked, [=]() {
			delete ui->tbList->currentItem();

			this->markAsChanged();
		});
	}

	// Load initial
	for (ToolbarRule *rule: g_sett->toolbarOverrides()) {
		rule->setParent(this);
		addTbRule(rule);
	}
}

void AeroThemeKCM::addTbRule(ToolbarRule *rule)
{
	auto *item = new QListWidgetItem();
	ui->tbList->addItem(item);

	item->setData(Qt::UserRole, QVariant::fromValue(rule));

	auto updateText = [=]() {
		QString s = "";
		s += rule->appName();
		if (!rule->objName().isEmpty()) {
			s += " / ";
			s += rule->objName();
		}
		item->setText(s);
	};
	connect(rule, &ToolbarRule::appNameChanged, updateText);
	connect(rule, &ToolbarRule::objNameChanged, updateText);

	updateText();
}

void AeroThemeKCM::save()
{
	{
		QList<ToolbarRule *> arr;

		for (int i = 0; i < ui->tbList->count(); i++) {
			auto *item = ui->tbList->item(i);
			auto *rule = item->data(Qt::UserRole).value<ToolbarRule *>();

			arr.append(rule);
		}

		g_sett->setToolbarOverrides(arr);
	}

	g_sett->save();

	KCModule::save();
}

AeroThemeKCM::~AeroThemeKCM()
{
	delete ui;
}


#include "aerothemekcm.moc"
