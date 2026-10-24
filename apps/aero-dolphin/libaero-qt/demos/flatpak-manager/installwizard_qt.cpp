#include "installwizard_qt.h"
#include "ui_installwizard_qt.h"

InstallWizard_Qt::InstallWizard_Qt(QWidget *parent)
    : Aero::Wizard(parent)
	, ui(new Ui::InstallWizard_Qt)
{
	ui->setupUi(this);
}

InstallWizard_Qt::~InstallWizard_Qt()
{
	delete ui;
}
