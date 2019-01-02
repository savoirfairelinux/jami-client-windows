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

#include "lrcinstance.h"
#include "api/newaccountmodel.h"

namespace Ui {
    class RegNameDialog;
}

class RegNameDialog : public QDialog
{
    Q_OBJECT
    RegNameDialog(const RegNameDialog& cpy);

public:
    RegNameDialog(const QString& newRegName = "", QWidget* parent = nullptr);
    ~RegNameDialog();

private slots:
    void nameRegistrationResultSlot(const std::string& accountId,
        lrc::api::account::RegisterNameStatus status, const std::string& registerdName);
    void startNameRegistration();

private:
    Ui::RegNameDialog* ui;
    QString registeredName_;
    QMovie* gif;

    void startSpinner();
};
