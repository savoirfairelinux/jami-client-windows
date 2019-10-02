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
    explicit NotifyPushButton(QWidget* parent = nullptr);
    ~NotifyPushButton();

    // geo
    void setNotifyCircleRadiusMakeup(int makeup) { circleRadiusMakeup_ = makeup; }
    void setNotifyCircleRadiusDivideRatio(int ratio) { circleRadiusDivideRatio_ = ratio; }
    void setNotifyCircleGeoPresenceMakeup(int makeup) { notifyCirclePresenceMakeup_ = makeup; }
    void setNotifyTextCorrectionY(int size) { notifyTextCorrectionY_ = size; }
    void setNotifyTextCorrectionX(int size) { notifyTextCorrectionX_ = size; }
    void setNotifyTextCorrectionXOverLimit(int makeup) { notifyTextCorrectionXOverLimit_ = makeup; }
    // count
    void setNotifyNumber(int num) { newCount_ = num; }
    int getNotifyNumber() { return newCount_; }
    void setNotifyLimit(int limit) { notifyLimit_  = limit; }
    // font size
    void setNotifyNumberFontSize(int size) { notifyNumberFontSize_ = size; }
    void setNotifyFontSizeDeductionOverLimit(int size) { notifyNumberFontSizeDeductionOverLimit_ = size; }

    void setNeedToShowNotify(bool need) { needToNotify_ = need; }
    void setGreyStyleNotification(bool grey) { greyStyleNotification_ = grey; }

protected:
    void paintEvent(QPaintEvent* e);

private:
    // count
    int newCount_ { 9 };
    int notifyLimit_{ 9 };
    // geo
    int circleRadiusDivideRatio_ { 6 };
    int circleRadiusMakeup_ { 1 };
    int notifyCirclePresenceMakeup_{ 6 };
    int notifyTextCorrectionY_{ 3 };
    int notifyTextCorrectionX_{ 8 };
    int notifyTextCorrectionXOverLimit_{ 2 };
    // font size
    int notifyNumberFontSize_ { 10 };
    int notifyNumberFontSizeDeductionOverLimit_ { 2 };

    bool needToNotify_ { true };
    bool greyStyleNotification_ { false };
};
