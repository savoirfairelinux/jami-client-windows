#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "navstack.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
//    void callIncoming(Call *call);
    //void on_configurationButton_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    NavStack* navStack_;


};

#endif // MAINWINDOW_H
