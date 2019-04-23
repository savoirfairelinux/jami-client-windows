/**************************************************************************
* Copyright (C) 2019-2019 by Savoir-faire Linux                           *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
#include "passworddialog.h"
#include "ui_passworddialog.h"

#include <QTimer>
#include <QtConcurrent/QtConcurrent>

#include "lrcinstance.h"

PasswordDialog::PasswordDialog(QWidget* parent)
    :ui(new Ui::PasswordDialog),
    QDialog(parent)
{
    ui->setupUi(this);

    ui->currentPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    ui->currentPasswordEdit->setEnabled(LRCInstance::getCurrAccConfig().archiveHasPassword);

    connect(ui->currentPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
    connect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
    connect(ui->btnChangePasswordConfirm, &QPushButton::clicked, [this] { savePassword(); });
    connect(ui->btnChangePasswordCancel, &QPushButton::clicked, [this] { reject(); });

    ui->btnChangePasswordConfirm->setEnabled(false);

    ui->wrongPasswordLabel->hide();

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void
PasswordDialog::validatePassword()
{
    bool acceptablePassword = ui->passwordEdit->text() == ui->confirmPasswordEdit->text();
    ui->btnChangePasswordConfirm->setEnabled(acceptablePassword);

    if (acceptablePassword) {
        ui->passwordEdit->setStyleSheet("border: 2px solid green;");
        ui->confirmPasswordEdit->setStyleSheet("border: 2px solid green;");
        return;
    }

    ui->passwordEdit->setStyleSheet("border: 2px solid red;");
    ui->confirmPasswordEdit->setStyleSheet("border: 2px solid red;");
}

void
PasswordDialog::savePassword()
{
    if (LRCInstance::editableAccountModel()->changeAccountPassword(LRCInstance::getCurrAccId(),
        ui->currentPasswordEdit->text().toStdString(), ui->passwordEdit->text().toStdString())) {

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.archiveHasPassword = !ui->passwordEdit->text().isEmpty();
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);

        accept();
    } else {
        ui->wrongPasswordLabel->show();
        ui->currentPasswordEdit->setText("");
    }
}