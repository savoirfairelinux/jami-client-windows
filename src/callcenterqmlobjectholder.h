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

class CallCenterQmlObjectHolder : public QObject
{
    Q_OBJECT

public:
    explicit CallCenterQmlObjectHolder(QObject *parent = nullptr);
    ~CallCenterQmlObjectHolder();

    // Must call Q_INVOKABLE so that this function can be used in QML, qml to c++
    Q_INVOKABLE void setCallCenterQmlObjectHolder(QObject *obj);
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
    void callStatusChanged(const QString &status, const QString &accountId, const QString &convUid);
    void closeCallWindow(const QString &accountId, const QString &convUid);
    void updateConversationSmartList();

public slots:
    void slotShowIncomingCallView(const QString &accountId,
                                  const lrc::api::conversation::Info &convInfo);
    void slotShowCallView(const QString &accountId, const lrc::api::conversation::Info &convInfo);

private:
    // Object pointer
    QObject *callCenterQmlObject_;

    QMetaObject::Connection callStatusChangedConnection_;
};
