/**************************************************************************
* Copyright (C) 2018 by Savoir-faire Linux                           *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include "regnamedialog.h"
#include "ui_regnamedialog.h"

#include <QTimer>

RegNameDialog::RegNameDialog(const QString& newRegName, QWidget* parent)
    :QDialog(parent),
    ui(new Ui::RegNameDialog),
    registeredName_(newRegName),
    gif(new QMovie(":/images/ajax-loader.gif"))
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->startPage);
    ui->registeredName->setText(newRegName);

    connect(ui->startPageConfirmBtn, &QPushButton::clicked, this, &RegNameDialog::startNameRegistration);

    connect(ui->startPageCancelBtn, &QPushButton::clicked, [this]() {
        reject();
        }
    );

    // get name registration result
    connect(LRCInstance::editableAccountModel(), &lrc::api::NewAccountModel::nameRegistrationEnded,
        this, &RegNameDialog::nameRegistrationResultSlot);
}

RegNameDialog::~RegNameDialog()
{
    disconnect(this);
    delete ui;
}

void
RegNameDialog::startNameRegistration()
{
    LRCInstance::editableAccountModel()->registerName(LRCInstance::getCurrAccId(),
        "", registeredName_.toStdString());
    startSpinner();
}

void
RegNameDialog::nameRegistrationResultSlot(const std::string& accountId,
    lrc::api::account::RegisterNameStatus status, const std::string& registerdName)
{
    gif->stop();

    if(status == lrc::api::account::RegisterNameStatus::SUCCESS) {
        ui->stackedWidget->setCurrentWidget(ui->nameRegisteredPage);
        QTimer::singleShot(1000, this, &RegNameDialog::accept);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->nameNotRegisteredPage);
        QTimer::singleShot(1000, this, &RegNameDialog::reject);
    }
}

void
RegNameDialog::startSpinner()
{
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);

    ui->spinnerLabel->setMovie(gif);
    gif->start();
}