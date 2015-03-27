#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    navStack_ = new NavStack(ui->bar, ui->stackedWidgetView);


}

MainWindow::~MainWindow()
{
    delete ui;
}
