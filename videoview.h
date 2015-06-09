/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H

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
    explicit VideoView(QWidget *parent = 0);
    ~VideoView();

protected:
    void resizeEvent(QResizeEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mouseDoubleClickEvent(QMouseEvent *e);

private slots:
    void callStateChanged(Call *call, Call::State previousState);
    void updateTimer();
    void showContextMenu(const QPoint &pos);

private:
    Ui::VideoView *ui;
    QTimer* timerLength_;
    VideoOverlay* overlay_;
    constexpr static int fadeOverlayTime_ = 2000; //msec
    QPropertyAnimation* fadeAnim_;
    QWidget *oldParent_;
    QSize oldSize_;
private:
    void toggleFullScreen();
};

#endif // VIDEOVIEW_H
