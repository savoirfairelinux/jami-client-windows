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

    connect(ui->currentPsswdEdit, &QLineEdit::textChanged, [=](const QString& text) {
        updatePasswordDialog(text); }
    );

    connect(ui->newPsswdEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateNewPsswd);
    connect(ui->confirmNewPsswdEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateNewPsswd);

    connect(ui->confirmButton, &QPushButton::clicked, [=]() {
        savePassword();
        }
    );

    connect(ui->backToSettingsButton, &QPushButton::clicked, [=]() {
        done(0);
        }
    );

    connect(ui->backToSettingsButton_2, &QPushButton::clicked, [=]() {
        done(0);
        }
    );

    connect(ui->cancelButton, &QPushButton::clicked, [=]() {
        done(0);
        }
    );

    updatePasswordDialog("");
    ui->confirmButton->setEnabled(false);
}

PasswordDialog::~PasswordDialog()
{
    disconnect(this);
    delete ui;
}

void
PasswordDialog::updatePasswordDialog(const QString& text)
{
    ui->currentPsswdEdit->setStyleSheet("border: 1px solid red;");

    std::string psswd = text.toStdString();
    if (LRCInstance::currentAccountPasswordMatch(psswd)) {
        ui->currentPsswdEdit->setStyleSheet("border: 1px solid green;");
    }
    freeConfirmButton();
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
    if (ui->newPsswdEdit->text() == ui->confirmNewPsswdEdit->text() &&
        LRCInstance::currentAccountPasswordMatch(ui->currentPsswdEdit->text().toStdString())) {
        ui->confirmButton->setEnabled(true);
    }
    else {
        ui->confirmButton->setEnabled(false);
    }
}

void
PasswordDialog::savePassword()
{
    // if change successfull
    if (!LRCInstance::changeCurrentPassword(LRCInstance::getSelectedAccountId(),
        ui->currentPsswdEdit->text().toStdString(), ui->newPsswdEdit->text().toStdString(),
        ui->confirmNewPsswdEdit->text().toStdString())) {
        ui->stackedWidget->setCurrentWidget(ui->psswdSuccessfullyChanged);
    }
    else { // if change unsuccessfull
        ui->stackedWidget->setCurrentWidget(ui->psswdFailedToChange);
    }
}