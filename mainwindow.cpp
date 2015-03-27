#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSizeGrip>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIcon icon(":/ring.png");

    setWindowIcon(icon);

    sysIcon_.setIcon(icon);
    sysIcon_.show();

    connect(&sysIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->verticalLayout_2->addWidget(new QSizeGrip(this), 0, Qt::AlignBottom | Qt::AlignRight);

    navStack_ = new NavStack(ui->bar, ui->stackedWidgetView, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *evt)
{
    oldPos_ = evt->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *evt)
{
    if(evt->buttons() & Qt::LeftButton) {
        const QPoint delta = evt->globalPos() - oldPos_;
        move(x() + delta.x(), y() + delta.y());
        oldPos_ = evt->globalPos();
    }
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason != QSystemTrayIcon::ActivationReason::Context)
        this->show();
}
