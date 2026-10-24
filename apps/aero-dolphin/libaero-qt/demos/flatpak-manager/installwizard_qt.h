#ifndef INSTALLWIZARD_QT_H
#define INSTALLWIZARD_QT_H

#include <QWizard>
#include <AeroQt/wizard.h>

namespace Ui {
class InstallWizard_Qt;
}

class InstallWizard_Qt : public Aero::Wizard
{
    Q_OBJECT

public:
    explicit InstallWizard_Qt(QWidget *parent = nullptr);
    ~InstallWizard_Qt();

private:
    Ui::InstallWizard_Qt *ui;
};

#endif // INSTALLWIZARD_QT_H
