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

    ui->stackedWidget->setCurrentWidget(ui->startPage);

    ui->registerBtn->setEnabled(false);

    connect(ui->registerBtn, &QPushButton::clicked, this, &NameRegistrationDialog::slotStartNameRegistration);

    connect(ui->cancelBtn, &QPushButton::clicked,
        [this]() {
            reject();
        });

    connect(LRCInstance::editableAccountModel(), &lrc::api::NewAccountModel::nameRegistrationEnded,
        this, &NameRegistrationDialog::slotNameRegistrationResult);

    spinnerAnimation_ = new QMovie(":/images/jami_rolling_spinner.gif");
}

NameRegistrationDialog::~NameRegistrationDialog()
{
    disconnect(this);
    delete ui;
}

void
NameRegistrationDialog::slotStartNameRegistration()
{
    auto password = ui->passwordEdit->text().toStdString();
    LRCInstance::editableAccountModel()->registerName(LRCInstance::getCurrAccId(),
        password, nameToRegister_.toStdString());
    startSpinner();
}

void
NameRegistrationDialog::slotNameRegistrationResult(const std::string& accountId,
    lrc::api::account::RegisterNameStatus status, const std::string& registerdName)
{
    Q_UNUSED(accountId);
    Q_UNUSED(registerdName);

    spinnerAnimation_->stop();

    if(status == lrc::api::account::RegisterNameStatus::SUCCESS) {
        accept();
    } else {
        ui->stackedWidget->setCurrentWidget(ui->nameNotRegisteredPage);
        QTimer::singleShot(1000, this, &NameRegistrationDialog::reject);
    }
}

void
NameRegistrationDialog::startSpinner()
{
    ui->stackedWidget->setCurrentWidget(ui->loadingPage);

    ui->spinnerLabel->setMovie(spinnerAnimation_);
    spinnerAnimation_->start();
}