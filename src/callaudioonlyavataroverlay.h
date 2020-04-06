/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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

// qt
#include <QWidget>

// project
#include "lrcinstance.h"
#include "utils.h"

namespace Ui {
class CallAudioOnlyAvatarOverlay;
}

class CallAudioOnlyAvatarOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit CallAudioOnlyAvatarOverlay(QWidget *parent = nullptr);
    ~CallAudioOnlyAvatarOverlay();
    void setAvatarVisible(bool visible);
    void writeAvatarOverlay(const lrc::api::conversation::Info& convInfo);
    void respondToPauseLabel(bool isPaused);

private:
    Ui::CallAudioOnlyAvatarOverlay *ui;
};
