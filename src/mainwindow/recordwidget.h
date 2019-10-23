/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
* Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "../misc/widgethelpers.h"
#include "recordoverlay.h"
#include "previewwidget.h"

#include <QWidget>

namespace Ui {
class RecordWidget;
}

class RecordWidget final : public PopupWidget
{
    Q_OBJECT
public:
    explicit RecordWidget(QWidget *parent = nullptr);
    ~RecordWidget();

    bool startRecording();
    bool finishRecording();
    bool sendRecording();
    void recordAgain();
    void deleteRecording();

    void openRecorder(bool isAudio);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    Ui::RecordWidget* ui;

    VideoRecordPreviewWidget* previewWidget_;
    RecordOverlay* recordOverlay_;
    bool isAudio_ = true;
    QString recordedFilePath_;

};
