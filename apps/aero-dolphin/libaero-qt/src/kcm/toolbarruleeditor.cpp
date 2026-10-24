#include "toolbarruleeditor.h"
#include "ui_toolbarruleeditor.h"

#include <AeroQt/util/props.h>
#include <AeroQt/util/scopefn.h>

#include <QToolBar>

ToolbarRuleEditor::ToolbarRuleEditor(ToolbarRule *rule, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ToolbarRuleEditor)
{
	ui->setupUi(this);

	bind_prop_bidi(
		rule, "appName", &ToolbarRule::appNameChanged,
		ui->appName, "text", &QLineEdit::textChanged,
		true
	);

	bind_prop_bidi(
		rule, "objName", &ToolbarRule::objNameChanged,
		ui->objName, "text", &QLineEdit::textChanged,
		true
	);

	bind_prop_bidi(
		rule, "color", &ToolbarRule::colorChanged,
		ui->color, "currentText", &QComboBox::currentTextChanged,
		true
	);

	bind_prop_bidi(
		rule, "flat", &ToolbarRule::flatChanged,
		ui->flat, "checked", &QCheckBox::toggled,
		true
	);

	/* Preview */

	auto *toolBar = new QToolBar + also {
		it->addAction("Action");

		ui->tbFrame->layout()->addWidget(it);
	};

	auto restyleTb = [=]() {
		toolBar->setProperty("_Aero_styleclass", rule->color());
		toolBar->setProperty("_Aero_flat", rule->flat() ? "true" : "false");

		toolBar->style()->unpolish(toolBar);
		toolBar->style()->polish(toolBar);
		toolBar->update();
	};

	connect(rule, &ToolbarRule::colorChanged, restyleTb);
	connect(rule, &ToolbarRule::flatChanged, restyleTb);
	restyleTb();
}

ToolbarRuleEditor::~ToolbarRuleEditor()
{
	delete ui;
}
