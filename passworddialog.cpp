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
#include <QMessageBox>

#include "lrcinstance.h"

PasswordDialog::PasswordDialog(QWidget* parent, PasswordEnteringPurpose purpose)
    :ui(new Ui::PasswordDialog),
    purpose_(purpose),
    QDialog(parent)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());

    ui->currentPasswordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    ui->currentPasswordEdit->setEnabled(LRCInstance::getCurrAccConfig().archiveHasPassword);

    if (purpose_ == PasswordEnteringPurpose::ChangePassword) {
        connect(ui->currentPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->passwordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->btnChangePasswordConfirm, &QPushButton::clicked, [this] { savePassword(); });
    } else if (purpose_ == PasswordEnteringPurpose::ExportAccount) {
        setWindowTitle(tr("Enter the password of this account"));
        ui->passwordEdit->hide();
        ui->confirmPasswordEdit->hide();
        connect(ui->btnChangePasswordConfirm, &QAbstractButton::clicked, [this] { exportAccount(); });
        connect(ui->currentPasswordEdit, &QLineEdit::textChanged,
            [this] {
                if (ui->currentPasswordEdit->text().isEmpty()) {
                    ui->btnChangePasswordConfirm->setEnabled(false);
                } else {
                    ui->btnChangePasswordConfirm->setEnabled(true);
                }
            });
    }

    connect(ui->btnChangePasswordCancel, &QPushButton::clicked, [this] { reject(); });
    ui->btnChangePasswordConfirm->setEnabled(false);
    ui->wrongPasswordLabel->hide();
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void
PasswordDialog::exportAccount()
{
    bool success = LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(),
                                                              path_,
                                                              ui->currentPasswordEdit->text().toStdString());
    if(success) {
        QMessageBox::information(0, tr("Success"), tr("Export Successful"));
        done(0);
    }
    ui->wrongPasswordLabel->show();
    ui->currentPasswordEdit->clear();
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
    if (LRCInstance::accountModel().changeAccountPassword(LRCInstance::getCurrAccId(),
        ui->currentPasswordEdit->text().toStdString(), ui->passwordEdit->text().toStdString())) {

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.archiveHasPassword = !ui->passwordEdit->text().isEmpty();
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

        accept();
    } else {
        ui->wrongPasswordLabel->show();
        ui->currentPasswordEdit->setText("");
    }
}
