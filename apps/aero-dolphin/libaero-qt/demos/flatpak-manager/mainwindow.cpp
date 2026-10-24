#include <QApplication>
#include <QtWidgets>

#include <AeroQt/stylesheet.h>
#include <AeroQt/navbuttons.h>
#include <AeroQt/insetwindow.h>
#include <AeroQt/usericon.h>
#include <AeroQt/util/objecteventlistener.h>
#include <AeroQt/util/scopefn.h>

#include "installwizard_qt.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto nb = new Aero::NavButtons();
    nb->forward()->setEnabled(false);
    nb->back()->setEnabled(false);
    // auto m = new QMenu(nb); nb->setMenu(m);
    ui->toolBar->insertWidget(nullptr, nb);
    ui->toolBar->insertWidget(nullptr, new QPushButton("Hello"));

    connect(ui->aInstallFlatpak, &QAction::triggered, [&]() {
        auto i = new InstallWizard_Qt(nullptr);
        i->show();
    });

    bind_prop(ui->showMenubarBox, "checked", menuBar(), "visible", &QAbstractButton::toggled, true);

    //

    ui->is->setIcon(QIcon::fromTheme("system-software-install"));

    //

    // ui->notif->setAction(new QAction(QIcon::fromTheme("weather-few-clouds"), "Testing", this) + also {
    //     connect(it, &QAction::triggered, [=]() {
    //         QMessageBox::information(this, "Title Text", "Your message goes here.");
    //     });
    //     it->setProperty("elevatedPriv", true);
    // });
    connect(ui->pushButton_4, &QAbstractButton::clicked, [=]() {
        // ui->notif->show();
        ui->kmsg->animatedShow();
    });

    //

    bind_prop(ui->slid, "value", ui->kcapb, "value", &QAbstractSlider::valueChanged, true);

    //

    ui->lay->insertWidget(0, new Aero::UserIcon(Aero::UserIcon::Size_60) + also {
        it->setIcon(QIcon::fromTheme("avatar-default"));
    });

    //

    ui->ap->setTitleAction(ui->aActions + also {
        // it->setToolTip("The actions available to this application.");
    });
    ui->ap->addAction(ui->aInstallFlatpak + also { it->setProperty("elevatedPriv", true); });
    ui->ap->addAction(ui->aWeather);
    ui->ap->addAction(ui->aTest);

    //

    ui->sb->addAction(ui->aInstallFlatpak);
    ui->sb->addAction(ui->aTest);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    Aero::makeInsetWindow(this, ui->contents, ui->topInsetContents, ui->bottomInsetContents);
}

MainWindow::~MainWindow()
{
    delete ui;
}
