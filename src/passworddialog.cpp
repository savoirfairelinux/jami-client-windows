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
#include "utils.h"

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
        ui->verticalSpacerCurrentTNew->changeSize(20, 8, QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->verticalSpacerNewTConfirm->changeSize(20, 8, QSizePolicy::Fixed, QSizePolicy::Fixed);

        connect(ui->currentPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->passwordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->confirmPasswordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validatePassword);
        connect(ui->btnChangePasswordConfirm, &QPushButton::clicked,
            [this] {
                spinnerMovie_->start();
                ui->spinnerLabel->show();
                QtConcurrent::run([this] { savePassword(); });
            });
    } else if (purpose_ == PasswordEnteringPurpose::ExportAccount) {
        setWindowTitle(tr("Enter the password of this account"));
        ui->passwordEdit->hide();
        ui->confirmPasswordEdit->hide();
        ui->verticalSpacerCurrentTNew->changeSize(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        ui->verticalSpacerNewTConfirm->changeSize(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);

        connect(ui->btnChangePasswordConfirm, &QAbstractButton::clicked,
            [this] {
                spinnerMovie_->start();
                ui->spinnerLabel->show();
                QtConcurrent::run([this] { exportAccount(); });
            });
    }

    connect(ui->currentPasswordEdit, &QLineEdit::textChanged,
        [this] {
            if (ui->currentPasswordEdit->text().isEmpty()) {
                ui->btnChangePasswordConfirm->setEnabled(false);
            } else {
                ui->wrongPasswordLabel->hide();
                ui->btnChangePasswordConfirm->setEnabled(true);
            }
        });
    connect(ui->btnChangePasswordCancel, &QPushButton::clicked, [this] { reject(); });

    ui->btnChangePasswordConfirm->setEnabled(false);
    ui->wrongPasswordLabel->hide();

    spinnerMovie_ = new QMovie(":/images/ajax-loader.gif");
    spinnerMovie_->setScaledSize(QSize(20, 20));
    ui->spinnerLabel->setMovie(spinnerMovie_);
    ui->spinnerLabel->hide();
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
                                                              ui->currentPasswordEdit->text());
    ui->spinnerLabel->hide();
    spinnerMovie_->stop();
    if(success) {
        done(SuccessCode);
    } else {
        Utils::whileBlocking<QLineEdit>(ui->currentPasswordEdit)->clear();
        ui->btnChangePasswordConfirm->setEnabled(false);
        ui->wrongPasswordLabel->show();
    }
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
    bool success = LRCInstance::accountModel().changeAccountPassword(LRCInstance::getCurrAccId(),
                   ui->currentPasswordEdit->text(), ui->passwordEdit->text());

    ui->spinnerLabel->hide();
    spinnerMovie_->stop();
    if (success) {

        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.archiveHasPassword = !ui->passwordEdit->text().isEmpty();
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

        done(SuccessCode);
    } else {
        Utils::whileBlocking<QLineEdit>(ui->currentPasswordEdit)->clear();
        ui->btnChangePasswordConfirm->setEnabled(false);
        ui->wrongPasswordLabel->show();
    }
}
