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

class CallOverlayQmlObjectHolder : public QObject
{
    Q_OBJECT

public:
    explicit CallOverlayQmlObjectHolder(QObject *parent = nullptr);
    ~CallOverlayQmlObjectHolder();

    // Must call Q_INVOKABLE so that this function can be used in QML, qml to c++
    Q_INVOKABLE void setCallOverlayQmlObjectHolder(QObject *obj);
    Q_INVOKABLE void updateCallOverlay(const QString &accountId, const QString &convUid);
    Q_INVOKABLE void hangUpThisCall();
    Q_INVOKABLE void holdThisCallToggle();
    Q_INVOKABLE void muteThisCallToggle();
    Q_INVOKABLE void recordThisCallToggle();

signals:
    void updateTimeText(const QString &time);
    void buttonStatusChanged(
        bool isPaused, bool isAudioOnly, bool isAudioMuted, bool isVideoMuted, bool isRecording);
    void showOnHoldLabel(bool isPaused);

private:
    void setTime(const QString &accountId, const QString &convUid);

    // Object pointer
    QObject *callOverlayQmlObject_;

    // for current conf/call info
    QString accountId_;
    QString convUid_;

    QTimer *oneSecondTimer_;
};
