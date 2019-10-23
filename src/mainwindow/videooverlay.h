/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "api/conversationmodel.h"

#include <QWidget>
#include <QMenu>
#include <QTimer>
#include <QPropertyAnimation>
#include <QTimer>

class ContactPicker;
class SipInputPanel;

namespace Ui {
class VideoOverlay;
}

using namespace lrc::api;

class VideoOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit VideoOverlay(QWidget* parent = 0);
    ~VideoOverlay();

    void updateCall(const conversation::Info& convInfo);
    void simulateShowChatview(bool checked);

signals:
    void setChatVisibility(bool visible);
    void holdStateChanged(bool state);
    void videoMuteStateChanged(bool state);

private slots:
    void fadeOverlayOut();
    void setTime();
    void on_hangupButton_clicked();
    void on_chatButton_toggled(bool checked);
    void on_holdButton_toggled(bool checked);
    void on_noMicButton_toggled(bool checked);
    void on_noVideoButton_toggled(bool checked);
    void on_recButton_clicked();
    void on_transferCallButton_toggled(bool checked);
    void on_addToConferenceButton_toggled(bool checked);
    void on_sipInputPanelButton_toggled(bool checked);
    void slotWillDoTransfer(const std::string& contactUri);
    void slotContactWillJoinConference(const std::string& contactUri);
    void slotCallWillJoinConference(const std::string& callId);
    void slotSIPInputPanelClicked(const int& id);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    Ui::VideoOverlay* ui;

    // for current conf/call info
    std::string accountId_;
    std::string convUid_;

    ContactPicker* contactPicker_;
    SipInputPanel* sipInputPanel_;
    QTimer* oneSecondTimer_;

    QTimer fadeTimer_;
    QPropertyAnimation* fadeAnim_;
    constexpr static int fadeOverlayTime_ = 1000; //msec
    // Time before the overlay starts fading out after the mouse stops
    // moving within the videoview.
    constexpr static int startfadeOverlayTime_ = 2000; //msec

    // TODO: fix when changing Qt version
    // Full(1.0) opacity bug affecting many Qt versions (macOS + win10)
    // causing the render to take a buggy code path which can be avoided
    // by using opacity values other than precisely 1.0.
    // https://bugreports.qt.io/browse/QTBUG-65981
    // https://bugreports.qt.io/browse/QTBUG-66803
    constexpr static qreal maxOverlayOpacity_ = 0.9999999999980000442;

    bool shouldShowOverlay();
    void showOverlay();
};
