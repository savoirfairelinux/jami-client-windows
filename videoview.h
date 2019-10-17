/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "callaudioonlyavataroverlay.h"
#include "videooverlay.h"
#include "previewwidget.h"

#include "api/conversationmodel.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

namespace Ui {
class VideoView;
}

using namespace lrc::api;

class VideoView : public QWidget {
    Q_OBJECT

public:
    explicit VideoView(QWidget* parent = 0);
    ~VideoView();

    void setupForConversation(const std::string& accountId,
                              const std::string& convUid);
    void showChatviewIfToggled();
    void simulateShowChatview(bool checked);
    void resetPreview();

protected:
    void resizeEvent(QResizeEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private slots:
    void slotCallStatusChanged(const std::string& callId);
    void showContextMenu(const QPoint& position);
    void slotVideoMuteStateChanged(bool state);

private:
    Ui::VideoView* ui;

    std::string accountId_;
    std::string convUid_;

    // preview
    PreviewSnap currentPreviewLocation_ = PreviewSnap::SE;
    VideoCallPreviewWidget* previewWidget_;
    constexpr static int previewMargin_ = 15;

    // video overlay
    VideoOverlay* overlay_;

    // audio only overlay
    // TODO: put this into the VideoOverlay class
    CallAudioOnlyAvatarOverlay* audioOnlyAvatar_;

    QMetaObject::Connection callStatusChangedConnection_;

    // dragging the preview
    QPropertyAnimation* moveAnim_;
    QPoint originMouseDisplacement_;
    bool draggingPreview_ = false;
    bool sharingEntireScreen_ = false;

    // dtmf
    int keyPressed_;

private:
    bool shouldShowPreview();
    void toggleFullScreen();

signals:
    void setChatVisibility(bool visible);
    void toggleFullScreenClicked();
    void terminating(const std::string& id);
};
