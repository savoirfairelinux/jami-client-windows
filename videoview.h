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

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QPropertyAnimation>

#include "videooverlay.h"

namespace Ui {
class VideoView;
}

class VideoView : public QWidget
{
    Q_OBJECT

public:
    explicit VideoView(QWidget* parent = 0);
    ~VideoView();
    void pushRenderer(Call* call);

protected:
    void resizeEvent(QResizeEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* e);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void callStateChanged(Call* call, Call::State previousState);
    void updateCall();
    void showContextMenu(const QPoint& pos);
    void slotVideoStarted(Video::Renderer* renderer);

private:
    Ui::VideoView* ui;
    VideoOverlay* overlay_;
    QPropertyAnimation* fadeAnim_;
    QWidget* oldParent_;
    QSize oldSize_;
    QMetaObject::Connection timerConnection_;
    QMetaObject::Connection videoStartedConnection_;
    QPoint origin_;
    QPoint originMouseDisplacement_;
    bool draggingPreview_ = false;
    bool resizingPreview_ = false;

    constexpr static int fadeOverlayTime_ = 2000; //msec
    constexpr static int resizeGrip_ = 40;
    constexpr static int minimalSize_ = 100;

private:
    void toggleFullScreen();
signals:
    void setChatVisibility(bool visible);
    void videoSettingsClicked();
};

