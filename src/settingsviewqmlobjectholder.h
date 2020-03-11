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

#include "lrcinstance.h"
#include "utils.h"

#include "bannedlistmodel.h"

#include "api/datatransfermodel.h"
#include "typedefs.h"

class SettingsViewQmlObjectHolder : public QObject
{
    Q_OBJECT
public:
    enum SettingsMenu { Account, General, Media };
    enum RegName { BLANK, INVALIDFORM, TAKEN, FREE, SEARCHING };
    Q_ENUM(SettingsMenu)
    Q_ENUM(RegName)

public:
    explicit SettingsViewQmlObjectHolder(QObject *parent = nullptr);

public:
    // functions to call in QML
    Q_INVOKABLE void setSettingsViewQmlObject(QObject *obj);

    // function wrappers for LRC
    Q_INVOKABLE bool getIsSIP();

    // set up variables in current class


    // regular utils function wrapper for qml


    // function wrappers for for handling signals and slots


signals:
    // show main view signal
    void showMainViewWindow();

    // property change notification signal

    // object holder signals
    void setRegNameUi(RegName stat);

private:
    QObject *settingsViewQmlObject;
};
