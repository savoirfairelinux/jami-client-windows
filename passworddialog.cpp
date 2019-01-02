/**************************************************************************
* Copyright (C) 2019-2019 by Savoir-faire Linux                           *
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
#include <QTimer>

#include "passworddialog.h"
#include "ui_passworddialog.h"

#include "lrcinstance.h"

PasswordDialog::PasswordDialog(QWidget* parent)
    :ui(new Ui::PasswordDialog),
    QDialog(parent)
{
    ui->setupUi(this);

    ui->currentPsswdEdit->setEchoMode(QLineEdit::Password);
    ui->newPsswdEdit->setEchoMode(QLineEdit::Password);
    ui->confirmNewPsswdEdit->setEchoMode(QLineEdit::Password);

    if (!LRCInstance::getCurrAccConfig().archiveHasPassword) {
        ui->currentPsswdEdit->hide();
    } else {
        ui->currentPsswdEdit->show();
    }

    connect(ui->currentPsswdEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateNewPsswd);
    connect(ui->newPsswdEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateNewPsswd);
    connect(ui->confirmNewPsswdEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateNewPsswd);

    connect(ui->confirmButton, &QPushButton::clicked, [=]() {
        savePassword();
        }
    );

    connect(ui->cancelButton, &QPushButton::clicked, this, &PasswordDialog::closeSlot);

    ui->confirmButton->setEnabled(false);
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

// only sets new password if both new passwords match
void
PasswordDialog::validateNewPsswd()
{
    ui->newPsswdEdit->setStyleSheet("border: 1px solid red;");
    ui->confirmNewPsswdEdit->setStyleSheet("border: 1px solid red;");

    if (ui->newPsswdEdit->text() == ui->confirmNewPsswdEdit->text()) {
        ui->newPsswdEdit->setStyleSheet("border: 1px solid green;");
        ui->confirmNewPsswdEdit->setStyleSheet("border: 1px solid green;");
    }
    freeConfirmButton();
}

void
PasswordDialog::freeConfirmButton()
{
    if (ui->newPsswdEdit->text() == ui->confirmNewPsswdEdit->text()) {
        ui->confirmButton->setEnabled(true);
    }
    else {
        ui->confirmButton->setEnabled(false);
    }
}

void
PasswordDialog::savePassword()
{
    if (LRCInstance::editableAccountModel()->changeAccountPassword(LRCInstance::getCurrAccId(),
        ui->currentPsswdEdit->text().toStdString(), ui->newPsswdEdit->text().toStdString())) {
        ui->userMessageLabel->setText(tr("Password Changed Successfully"));

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        ui->newPsswdEdit->text().isEmpty() ? confProps.archiveHasPassword = false : confProps.archiveHasPassword = true;
        LRCInstance::editableAccountModel()->setAccountConfig(LRCInstance::getCurrAccId(), confProps);

        ui->cancelButton->hide();
        ui->confirmButton->hide();
        ui->currentPsswdEdit->hide();
        ui->newPsswdEdit->hide();
        ui->confirmNewPsswdEdit->hide();

        QTimer::singleShot(900, this, SLOT(closeSlot()));
    }
    else {
        ui->userMessageLabel->setText(tr("Current Password Incorrect"));
        ui->currentPsswdEdit->setText("");
    }
}