/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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

#include "widgethelpers.h"

namespace Ui {
class RecordOverlay;
}

class RecordWidget;

class RecordOverlay : public VignetteWidget
{
    Q_OBJECT
    Q_PROPERTY(QPixmap pixRedDot READ getOriginPix WRITE setOriginPix DESIGNABLE true);

public:
    enum class RecorderState
    {
        aboutToRecord,
        recording,
        recorded
    };

    QPixmap getOriginPix();
    void setOriginPix(QPixmap);

public:
    explicit RecordOverlay(RecordWidget *recordWidget);
    ~RecordOverlay();

    void setRecorderState(RecorderState status);

private:
    void switchToAboutToRecordPage();
    void switchToRecordingPage();
    void switchToRecordedPage();

    void reinitializeTimer();

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private slots:
    void on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked);
    void on_recordOverlaySendBtn_pressed();
    void on_recordOverlayDeleteBtn_pressed();
    void on_recordOverlayRerecordBtn_pressed();

    void whenAudioRecordProgressUpdate();

private:
    Ui::RecordOverlay* ui;

    RecordWidget* recordWidget_;
    RecorderState recorderState_;
    int currentTime_ = 0;
    QTimer recordTimer_;
    QPixmap redDotPix_;
};