/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSizeGrip>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    QIcon icon(":/ring.png");

    this->setWindowIcon(icon);

    sysIcon_.setIcon(icon);
    sysIcon_.show();

    connect(&sysIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

    navStack_ = new NavStack(ui->bar, ui->stackedWidgetView, this);
    ui->verticalLayout_2->addWidget(
                new QSizeGrip(this), 0, Qt::AlignBottom | Qt::AlignRight);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::mousePressEvent(QMouseEvent *evt)
{
    oldPos_ = evt->globalPos();
}

void
MainWindow::mouseMoveEvent(QMouseEvent *evt)
{
    if(evt->buttons() & Qt::LeftButton) {
        const auto delta = evt->globalPos() - oldPos_;
        move(x() + delta.x(), y() + delta.y());
        oldPos_ = evt->globalPos();
    }
}

void
MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason != QSystemTrayIcon::ActivationReason::Context)
        this->show();
}
