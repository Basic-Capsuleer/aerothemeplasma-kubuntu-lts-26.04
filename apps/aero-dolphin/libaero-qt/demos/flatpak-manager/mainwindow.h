#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #undef signals
// #include <flatpak/flatpak.h>
// #define signals Q_SIGNALS

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // FlatpakInstallation *fp;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
