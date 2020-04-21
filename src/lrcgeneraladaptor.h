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

#include "lrcinstance.h"

class LrcGeneralAdaptor : public QObject
{
    Q_OBJECT
public:
    explicit LrcGeneralAdaptor(QObject *parent = nullptr);

    Q_INVOKABLE const QString getCurrAccId();
    Q_INVOKABLE const QStringList getCurrAccList();
    Q_INVOKABLE int getAccountListSize();
    Q_INVOKABLE void setCurrentCall(const QString &accountId, const QString &convUid);
    Q_INVOKABLE void startPreviewing(bool force);
    Q_INVOKABLE void stopPreviewing();
    Q_INVOKABLE bool hasVideoCall();
    Q_INVOKABLE const QString getCallId(const QString &accountId, const QString &convUid);

signals:
};
