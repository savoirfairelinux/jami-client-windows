/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include <QWidget>

namespace Ui {
class RecordOverlay;
}

class RecordOverlay : public QWidget
{
    Q_OBJECT

public:
    enum class RecorderStatus
    {
        aboutToRecord,
        recording,
        recorded
    };

public:
    // TODO: change container widget to the specific record widget
    explicit RecordOverlay(QWidget *containerWidget = nullptr,QWidget *parent = nullptr);
    ~RecordOverlay();

    void setUpRecorderStatus(RecorderStatus status);

private slots:
    void on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked);

    void on_recordOverlaySendBtn_pressed();
    void on_recordOverlayDeleteBtn_pressed();
    void on_recordOverlayRerecordBtn_pressed();

private:
    Ui::RecordOverlay* ui;

    RecorderStatus* status_;
};
