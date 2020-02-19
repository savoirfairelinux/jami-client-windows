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
#pragma once

#include <QDialog>
#include <QMovie>

namespace Ui {
    class PasswordDialog;
}

enum class PasswordEnteringPurpose{
    ChangePassword,
    ExportAccount
};

class PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    PasswordDialog(QWidget* parent = nullptr, PasswordEnteringPurpose purpose = PasswordEnteringPurpose::ChangePassword);
    ~PasswordDialog();

    static const int SuccessCode = 200;

    void setExportPath(const QString& path) { path_ = path; }

private slots:
    void validatePassword();
    void exportAccount();

private:
    Ui::PasswordDialog* ui;

    PasswordEnteringPurpose purpose_ { PasswordEnteringPurpose::ChangePassword };
    QString path_;
    QMovie* spinnerMovie_;

    void savePassword();
};
