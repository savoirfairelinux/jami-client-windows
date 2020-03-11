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

#pragma once

#include <QObject>

class PasswordDialogQMLObjectHolder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(PasswordEnteringPurpose purpose READ getPurpose WRITE setPurpose NOTIFY purposeChanged)
    Q_PROPERTY(QString exportPath READ getExportPath WRITE setExportPath NOTIFY exportPathChanged)
public:
    explicit PasswordDialogQMLObjectHolder(QObject *parent = nullptr);

    enum PasswordEnteringPurpose { ChangePassword, ExportAccount };
    Q_ENUM(PasswordEnteringPurpose)

    void exportAccount(QString password);
    void savePassword(QString oldPassword, QString newPassword);

public:
    // functions to call in QML
    Q_INVOKABLE void setPasswordDialogQmlObject(QObject *obj);

    Q_INVOKABLE void exportAccount_QML_CALL();
    Q_INVOKABLE void savePassword_QML_CALL();

signals:
    void hideSpinnerMovieLabel();
    void isSavingPasswordSuccess(bool success);
    void isExportingPasswordSuccess(bool success);

private:
    QObject *qmlobj_;

    PasswordEnteringPurpose purpose_{PasswordEnteringPurpose::ChangePassword};
    QString path_;
};
