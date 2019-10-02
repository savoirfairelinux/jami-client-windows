/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>             *
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

#include "utils.h"

#include <QColor>
#include <QImage>
#include <QPaintEvent>
#include <QPushButton>

class NotifyPushButton : public QPushButton {

    Q_OBJECT

public:
    // note that height should always be equal to the width for this button
    explicit NotifyPushButton(QWidget* parent = nullptr,
                              // count
                              int newCount = 0,
                              int notifyLimit = 9,
                              // geo
                              int circleRadiusDivideRatio = 5,
                              int circleRadiusMakeup = 0 ,
                              int notifyCirclePresenceMakeup = 8,
                              int notifyTextCorrectionY = 5,
                              int notifyTextCorrectionX = 11,
                              int notifyTextCorrectionXOverLimit = 2,
                              // font size
                              int notifyNumberFontSize = 10,
                              int notifyNumberFontSizeDeductionOverLimit = 2);
    ~NotifyPushButton();

    // count
    void setNotifyNumber(int num) { newCount_ = num; }
    int getNotifyNumber() { return newCount_; }
    void setNotifyLimit(int limit) { notifyLimit_ = limit; }

    void setNeedToShowNotify(bool need) { needToNotify_ = need; }
    void setGreyStyleNotification(bool grey) { greyStyleNotification_ = grey; }

protected:
    void paintEvent(QPaintEvent* e);

private:
    // count
    int newCount_;
    int notifyLimit_;
    // geo
    int circleRadiusDivideRatio_;
    int circleRadiusMakeup_;
    int notifyCirclePresenceMakeup_;
    int notifyTextCorrectionY_;
    int notifyTextCorrectionX_;
    int notifyTextCorrectionXOverLimit_;
    // font size
    int notifyNumberFontSize_;
    int notifyNumberFontSizeDeductionOverLimit_;

    bool needToNotify_ { true };
    bool greyStyleNotification_ { false };
};
