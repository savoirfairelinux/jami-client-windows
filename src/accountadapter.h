/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>
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

#include "qmladapterbase.h"

#include <QObject>
#include <QString>

class AccountAdapter : public QmlAdapterBase
{
    Q_OBJECT

public:
    explicit AccountAdapter(QObject *parent = 0);
    ~AccountAdapter();

    /*
     * Change to account corresponding to combox box index.
     */
    Q_INVOKABLE void accountChanged(int index);

    void setSelectedAccount(const QString &accountId, int index);
    void backToWelcomePage(int index);
    void deselectConversation();

signals:
    /*
     * Trigger other components to reconnect account related signals .
     */
    void accountSignalsReconnect(const QString &accountId);
    void accountStatusChanged();
    void updateConversationForAddedContact();

private:
    void initQmlObject() override final;
    /*
     * Make account signal connections.
     */
    void connectAccount(const QString &accountId);

    QMetaObject::Connection accountStatusChangedConnection_;
    QMetaObject::Connection contactAddedConnection_;
    QMetaObject::Connection addedToConferenceConnection_;
};