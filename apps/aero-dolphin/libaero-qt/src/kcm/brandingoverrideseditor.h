#ifndef BRANDINGOVERRIDESEDITOR_H
#define BRANDINGOVERRIDESEDITOR_H

#include <QDialog>

namespace Ui {
class BrandingOverridesEditor;
}

class BrandingOverridesEditor : public QDialog
{
	Q_OBJECT

	Ui::BrandingOverridesEditor *ui;

public:
	explicit BrandingOverridesEditor(QWidget *parent = nullptr);
	~BrandingOverridesEditor();

private:
	void load(QString path);
	void save(QString path);
};

#endif // BRANDINGOVERRIDESEDITOR_H
