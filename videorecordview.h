/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include <QWidget>
#include <QPropertyAnimation>
#include <QtMultimedia/QMediaPlayer>

namespace Ui {
class VideoRecordView;
}

class VideoRecordView : public QWidget
{
    Q_OBJECT

public:
    explicit VideoRecordView(QWidget *parent = nullptr);
    ~VideoRecordView();

signals:
    void recordControllerShouldCloseSignal();

protected:
    void hideEvent(QHideEvent *event);

private:
    Ui::VideoRecordView *ui;

    // TODO: fix when changing Qt version
    // Full(1.0) opacity bug affecting many Qt versions (macOS + win10)
    // causing the render to take a buggy code path which can be avoided
    // by using opacity values other than precisely 1.0.
    // https://bugreports.qt.io/browse/QTBUG-65981
    // https://bugreports.qt.io/browse/QTBUG-66803
    constexpr static qreal maxOverlayOpacity_ = 0.9999999999980000442;
    constexpr static int fadeRedDotTime_ = 1000; //msec

    QString recordedVideoFilePath_;
    QPropertyAnimation* flickerFadeAnimation_;
    QTimer* recordTimer_;
    int currentTime_ { 0 };
    bool forceToStopAnimation_ { false };

    void reRecord();
    void resetState();
    void resetTimer();

private slots:
    void on_videoStartOrFinishRecordingBtn_toggled(bool checked);
    void on_videoSendBtn_pressed();
    void on_videoRerecordBtn_pressed();
    void on_videoDeleteBtn_pressed();
    void restartOrFinishAnimation();
    void updateTimer();
    void videoDeviceListChanged();
};
