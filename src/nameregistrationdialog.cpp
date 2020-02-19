/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "nameregistrationdialog.h"
#include "ui_nameregistrationdialog.h"

#include "utils.h"
#include "ringthemeutils.h"

#include <QTimer>

NameRegistrationDialog::NameRegistrationDialog(const QString& nameToRegister, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::NameRegistrationDialog),
    nameToRegister_(nameToRegister)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());

    connect(ui->btnRegister, &QPushButton::clicked, this, &NameRegistrationDialog::slotStartNameRegistration);
    connect(ui->btnCancel, &QPushButton::clicked, [this]() { reject(); });
    connect(ui->btnCloseRegisterDialog, &QPushButton::clicked, [this]() { reject(); });

    connect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::nameRegistrationEnded,
            this, &NameRegistrationDialog::slotNameRegistrationResult);

    spinnerAnimation_ = new QMovie(":/images/jami_eclipse_spinner.gif");
    spinnerAnimation_->setScaledSize(QSize(96, 96));

    if (LRCInstance::getCurrAccConfig().archiveHasPassword) {
        ui->stackedWidget->setCurrentWidget(ui->passwordConfirmPage);
    } else {
        slotStartNameRegistration();
    }
}

NameRegistrationDialog::~NameRegistrationDialog()
{
    delete ui;
}

void
NameRegistrationDialog::slotStartNameRegistration()
{
    auto password = ui->passwordEdit->text();
    LRCInstance::accountModel().registerName(LRCInstance::getCurrAccId(), password, nameToRegister_);
    startSpinner();
}

void
NameRegistrationDialog::slotNameRegistrationResult(const QString& accountId,
                                                   lrc::api::account::RegisterNameStatus status,
                                                   const QString& registerdName)
{
    Q_UNUSED(accountId);
    Q_UNUSED(registerdName);

    spinnerAnimation_->stop();

    if(status == lrc::api::account::RegisterNameStatus::SUCCESS) {
        accept();
    } else {
        switch (status) {
        case  lrc::api::account::RegisterNameStatus::WRONG_PASSWORD:
            ui->lblRegistrationError->setText(tr("Incorrect password"));
            break;
        case  lrc::api::account::RegisterNameStatus::NETWORK_ERROR:
            ui->lblRegistrationError->setText(tr("Network error"));
            break;
        default:
            break;
        }
        ui->stackedWidget->setCurrentWidget(ui->nameNotRegisteredPage);
    }
}

void
NameRegistrationDialog::startSpinner()
{
    ui->stackedWidget->setCurrentWidget(ui->registeringPage);

    ui->spinnerLabel->setMovie(spinnerAnimation_);
    spinnerAnimation_->start();
}
