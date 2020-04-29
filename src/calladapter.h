/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
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

#include "lrcinstance.h"
#include "qmladapterbase.h"

#include <QObject>
#include <QString>

class CallAdapter : public QmlAdapterBase
{
    Q_OBJECT

public:
    explicit CallAdapter(QObject *parent = nullptr);
    ~CallAdapter();

    Q_INVOKABLE void initQmlObject() override;
    Q_INVOKABLE void placeAudioOnlyCall();
    Q_INVOKABLE void placeCall();
    Q_INVOKABLE void hangUpACall(const QString &accountId, const QString &convUid);
    Q_INVOKABLE void refuseACall(const QString &accountId, const QString &convUid);
    Q_INVOKABLE void acceptACall(const QString &accountId, const QString &convUid);

    Q_INVOKABLE void connectCallstatusChangedSignal(const QString &accountId);

signals:
    void showOutgoingCallPage(const QString &accountId, const QString &convUid);
    void showIncomingCallPage(const QString &accountId, const QString &convUid);
    void showAudioCallPage(const QString &accountId, const QString &convUid);
    void showVideoCallPage(const QString &accountId, const QString &convUid, const QString &callId);
    void showCallStack(const QString &accountId, const QString &convUid);
    void closeCallStack(const QString &accountId, const QString &convUid);
    void closePotentialIncomingCallPageWindow(const QString &accountId, const QString &convUid);
    void callStatusChanged(const QString &status, const QString &accountId, const QString &convUid);
    void updateConversationSmartList();

    void incomingCallNeedToSetupMainView(const QString &accountId, const QString &convUid);

public slots:
    void slotShowIncomingCallView(const QString &accountId,
                                  const lrc::api::conversation::Info &convInfo);
    void slotShowCallView(const QString &accountId, const lrc::api::conversation::Info &convInfo);

private:
    QMetaObject::Connection callStatusChangedConnection_;
};
