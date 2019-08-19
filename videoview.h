/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include "lrcinstance.h"

#include "videooverlay.h"
#include "callaudioonlyavataroverlay.h"

namespace Ui {
class VideoView;
}

class VideoView : public QWidget {
    Q_OBJECT

public:
    explicit VideoView(QWidget* parent = 0);
    ~VideoView();
    void pushRenderer(const std::string& callUid, bool isSIP);
    void showChatviewIfToggled();
    void simulateShowChatview(bool checked);
    void setCurrentCalleeName(const QString& CalleeDisplayName);
    void resetVideoOverlay(bool isAudioMuted, bool isVideoMuted, bool isRecording, bool isHolding);
    void resetAvatarOverlay(bool enable);
    void writeAvatarOverlay(const std::string& accountId, const lrc::api::conversation::Info& convInfo);
    void HoldStatusChanged(bool pauseLabelStatus);
    void setOverlayDisplayMode(const lrc::api::conversation::Info& convInfo);

protected:
    void resizeEvent(QResizeEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void slotCallStatusChanged(const std::string& callId);
    void showContextMenu(const QPoint& pos);
    void fadeOverlayOut();
    void showOverlay();

private:
    Ui::VideoView* ui;
    VideoOverlay* overlay_;
    CallAudioOnlyAvatarOverlay* audioOnlyAvatar_;
    QPropertyAnimation* fadeAnim_;
    QTimer fadeTimer_;
    QWidget* oldParent_;
    QSize oldSize_;
    QMetaObject::Connection timerConnection_;
    QMetaObject::Connection callStatusChangedConnection_;
    QPoint origin_;
    QPoint originMouseDisplacement_;
    bool draggingPreview_ = false;
    bool resizingPreview_ = false;
    bool sharingEntireScreen_ = false;

    constexpr static int fadeOverlayTime_ = 1000; //msec
    constexpr static int resizeGrip_ = 40;
    constexpr static int minimalSize_ = 100;

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

private:
    void toggleFullScreen();
signals:
    void setChatVisibility(bool visible);
    void videoSettingsClicked();
    void toggleFullScreenClicked();
    void closing(const std::string& callid);

};
