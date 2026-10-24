#ifndef TOOLBARRULEEDITOR_H
#define TOOLBARRULEEDITOR_H

#include <QDialog>
#include <AeroQt/private/aerothemesettings.h>
#include "aerothemekcm.h"

namespace Ui {
class ToolbarRuleEditor;
}

class ToolbarRuleEditor : public QDialog
{
	Q_OBJECT

public:
	explicit ToolbarRuleEditor(ToolbarRule *rule, QWidget *parent = nullptr);
	~ToolbarRuleEditor();

private:
	Ui::ToolbarRuleEditor *ui;
};

#endif // TOOLBARRULEEDITOR_H
