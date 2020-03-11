/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>
#include <QSettings>

#include "api/account.h"
#include "api/datatransfermodel.h"
#include "lrcinstance.h"
#include "typedefs.h"
#include "utils.h"

class SettingsAdaptor : public QObject
{
    Q_OBJECT
public:
    explicit SettingsAdaptor(QObject *parent = nullptr);

    const Q_INVOKABLE lrc::api::account::Info &getCurrentAccountInfo();

    Q_INVOKABLE QString getDir_Document();
    Q_INVOKABLE QString getDir_Download();

    Q_INVOKABLE bool getSettingsValue_CloseOrMinimized();
    Q_INVOKABLE bool getSettingsValue_EnableNotifications();
    Q_INVOKABLE bool getSettingsValue_AutoUpdate();

    Q_INVOKABLE void setClosedOrMin(bool state);
    Q_INVOKABLE void setNotifications(bool state);
    Q_INVOKABLE void setUpdateAutomatic(bool state);
    Q_INVOKABLE void setRunOnStartUp(bool state);
    Q_INVOKABLE void setDownloadPath(QString dir);
};