/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "lrcinstance.h"

#include <QObject>
#include <QString>

// to ease the logic with the power of c++
class AccountComboBoxQmlObjectHolder : public QObject
{
    Q_OBJECT

public:
    explicit AccountComboBoxQmlObjectHolder(QObject *parent = 0);
    ~AccountComboBoxQmlObjectHolder();

    // Must call Q_INVOKABLE so that this function can be used in QML, qml to c++
    Q_INVOKABLE void setAccountComboBoxQmlObject(QObject *obj);
    Q_INVOKABLE void accountChanged(int index);

    void setSelectedAccount(const QString &accountId, int index);
    void backToWelcomePage(int index);
    void deselectConversation();

signals:
    void accountSignalsReconnect(const QString &accountId);

private:
    void connectAccount(const QString &accountId);

    // Object pointer
    QObject *accountComboBoxQmlObject_;

    lrc::api::profile::Type currentTypeFilter_{};
};