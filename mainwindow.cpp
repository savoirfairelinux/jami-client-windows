#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSystemTrayIcon>
#include <QSizeGrip>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon icon(":/ring.png");

    setWindowIcon(icon);

    QSystemTrayIcon sysIcon(icon);
    sysIcon.show();

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->verticalLayout_2->addWidget(new QSizeGrip(this), 0, Qt::AlignBottom | Qt::AlignRight);

    navStack_ = new NavStack(ui->bar, ui->stackedWidgetView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *evt)
{
    oldPos = evt->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *evt)
{
    if(evt->buttons() & Qt::LeftButton) {
        const QPoint delta = evt->globalPos() - oldPos;
        move(x()+delta.x(), y()+delta.y());
        oldPos = evt->globalPos();
    }
}
