/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "lrcinstance.h"

#include <QJsEngine>
#include <QQmlEngine>

using namespace lrc::api;

class LrcGeneralAdapter : public QObject
{
    Q_OBJECT;

public:
    explicit LrcGeneralAdapter(QObject *parent = 0);
    ~LrcGeneralAdapter();

    Q_INVOKABLE const QString getCurrAccId();
    Q_INVOKABLE const QStringList getCurrAccList();
    Q_INVOKABLE void setCurrentCall(const QString &accountId, const QString &convUid);
    Q_INVOKABLE void startPreviewing(bool force);
    Q_INVOKABLE void stopPreviewing();
};