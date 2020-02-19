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

#pragma once

#include <QDialog>
#include <QMovie>

#include "lrcinstance.h"
#include "api/newaccountmodel.h"

namespace Ui {
    class NameRegistrationDialog;
}

using namespace lrc::api::account;

class NameRegistrationDialog : public QDialog
{
    Q_OBJECT;

public:
    NameRegistrationDialog(const QString& nameToRegister, QWidget* parent = nullptr);
    ~NameRegistrationDialog();

private slots:
    void slotNameRegistrationResult(const QString& accountId,
                                    RegisterNameStatus status,
                                    const QString& registerdName);
    void slotStartNameRegistration();

private:
    Ui::NameRegistrationDialog* ui;

    QString nameToRegister_;
    QMovie* spinnerAnimation_;

    void startSpinner();
};
