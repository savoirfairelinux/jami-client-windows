/**************************************************************************
| Copyright (C) 2020 by Savoir-faire Linux                                |
| Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              |
|                                                                         |
| This program is free software; you can redistribute it and/or modify    |
| it under the terms of the GNU General Public License as published by    |
| the Free Software Foundation; either version 3 of the License, or       |
| (at your option) any later version.                                     |
|                                                                         |
| This program is distributed in the hope that it will be useful,         |
| but WITHOUT ANY WARRANTY; without even the implied warranty of          |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
| GNU General Public License for more details.                            |
|                                                                         |
| You should have received a copy of the GNU General Public License       |
| along with this program.  If not, see <https://www.gnu.org/licenses/>.  |
**************************************************************************/

#pragma once

#include <QDialog>

#include "lrcinstance.h"

namespace Ui {
class SipCredentialDialog;
}

class SipCredentialDialog : public QDialog
{
    Q_OBJECT

public:
    enum class EditMode
    {
        NewCredential,
        EditCredential
    };

    explicit SipCredentialDialog(lrc::api::profile::Info currentSIPAccountInfo,
                                 QWidget* parent = nullptr,
                                 int itemIndex = 0,
                                 EditMode mode = EditMode::NewCredential);
    ~SipCredentialDialog();

    // key from daemon, since there is a confilct when adding account_const.h
    static const char* usernameKey;
    static const char* passwordKey;
    static const char* realmKey;

signals:
    void sipCredInfoChanged(const QString& username, const QString& password, const QString& realm);
    void sipCredInfoCreated(const QString& username, const QString& password, const QString& realm);

private:
    Ui::SipCredentialDialog*ui;
};
