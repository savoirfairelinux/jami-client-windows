/***************************************************************************
* Copyright (C) 2019-2020 by Savoir-faire Linux                            *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                       *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/

#include "passworddialogqmlobjectholder.h"

#include <QtConcurrent/QtConcurrent>

#include "lrcinstance.h"

PasswordDialogQMLObjectHolder::PasswordDialogQMLObjectHolder(QObject *parent)
    : QObject(parent)
{}

bool
PasswordDialogQMLObjectHolder::exportAccount(QString password, QString exportPath)
{
    bool success = LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(),
                                                            path_,
                                                            password);
    emit hideSpinnerMovieLabel();
    emit isExportingPasswordSuccess(success);
}

bool
PasswordDialogQMLObjectHolder::savePassword(QString oldPassword, QString newPassword)
{
    bool success = LRCInstance::accountModel().changeAccountPassword(LRCInstance::getCurrAccId(),
                                                                     oldPassword,
                                                                     newPassword);

    /*emit hideSpinnerMovieLabel();
    if (success) {
        auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
        confProps.archiveHasPassword = !ui->passwordEdit->text().isEmpty();
        LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);

        done(SuccessCode);
    } else {
        Utils::whileBlocking<QLineEdit>(ui->currentPasswordEdit)->clear();
        ui->btnChangePasswordConfirm->setEnabled(false);
        ui->wrongPasswordLabel->show();
    }*/
}

Q_INVOKABLE void
PasswordDialogQMLObjectHolder::setArchiveHasPassword(bool isHavePassword)
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    confProps.archiveHasPassword = isHavePassword;
    LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), confProps);
}

Q_INVOKABLE void
PasswordDialogQMLObjectHolder::setPasswordDialogQmlObject(QObject *obj)
{
    qmlobj_ = obj;
}
