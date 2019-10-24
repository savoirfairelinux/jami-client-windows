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

#include "notifypushbutton.h"

#include "lrcinstance.h"

#include <QPainter>
#include <QPainterPath>

NotifyPushButton::NotifyPushButton(QWidget* parent,
                                   // count
                                   int newCount,
                                   int notifyLimit,
                                   // geo
                                   int circleRadiusDivideRatio,
                                   int circleRadiusMakeup,
                                   int notifyCirclePresenceMakeup,
                                   int notifyTextCorrectionY,
                                   int notifyTextCorrectionX,
                                   int notifyTextCorrectionXOverLimit,
                                   // font size
                                   int notifyNumberFontSize,
                                   int notifyNumberFontSizeDeductionOverLimit) :
    QPushButton(parent),
    newCount_(newCount),
    notifyLimit_(notifyLimit),
    circleRadiusDivideRatio_(circleRadiusDivideRatio),
    circleRadiusMakeup_(circleRadiusMakeup),
    notifyCirclePresenceMakeup_(notifyCirclePresenceMakeup),
    notifyTextCorrectionY_(notifyTextCorrectionY),
    notifyTextCorrectionX_(notifyTextCorrectionX),
    notifyTextCorrectionXOverLimit_(notifyTextCorrectionXOverLimit),
    notifyNumberFontSize_(notifyNumberFontSize),
    notifyNumberFontSizeDeductionOverLimit_(notifyNumberFontSizeDeductionOverLimit)
{

}

NotifyPushButton::~NotifyPushButton()
{

}

void
NotifyPushButton::paintEvent(QPaintEvent* e)
{
    if (!needToNotify_) {
        setEnabled(false);
        return;
    }

    QPushButton::paintEvent(e);

    // set up
    bool isOverNotifyLimit = (newCount_ > notifyLimit_) ? true : false;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QFont font;
    font.setPixelSize(isOverNotifyLimit ? notifyNumberFontSize_ - notifyNumberFontSizeDeductionOverLimit_ : notifyNumberFontSize_);
    font.setWeight(QFont::Bold);

    QPainterPath notifyCircle;
    QPointF presenceCenter(height() - notifyCirclePresenceMakeup_, height() - notifyCirclePresenceMakeup_);
    qreal circleRadius = height() / circleRadiusDivideRatio_ + circleRadiusMakeup_;

    // draw circle
    notifyCircle.addEllipse(presenceCenter, circleRadius, circleRadius);
    painter.fillPath(notifyCircle, greyStyleNotification_ ? QBrush(QColor("grey")) : QBrush(QColor("red")));

    // draw notify count
    presenceCenter.setY(height() - notifyTextCorrectionY_);
    presenceCenter.setX(isOverNotifyLimit ?
                        height() - notifyTextCorrectionX_ - notifyTextCorrectionXOverLimit_:
                        height() - notifyTextCorrectionX_);
    painter.setPen(QPen(QColor("white")));
    painter.setOpacity(1);
    painter.setFont(font);

    QString notifyCount = (isOverNotifyLimit) ? (QString::number(notifyLimit_) + "+") : QString::number(newCount_);

    painter.drawText(presenceCenter, notifyCount);
}
