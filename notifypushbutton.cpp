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

NotifyPushButton::NotifyPushButton(QWidget* parent) :
    QPushButton(parent)
{

}

NotifyPushButton::~NotifyPushButton()
{

}

void
NotifyPushButton::paintEvent(QPaintEvent* e)
{
    QPainterPath outerCircle, innerCircle;
    QPointF presenceCenter(avatarSize_ + avatarTopLeft.x() - 9, avatarSize_ - 3);
    qreal outerCircleRadius = avatarSize_ / 6;
    qreal innerCircleRadius = outerCircleRadius * 0.75;
    outerCircle.addEllipse(presenceCenter, outerCircleRadius, outerCircleRadius);
    innerCircle.addEllipse(presenceCenter, innerCircleRadius, innerCircleRadius);
    painter.fillPath(outerCircle, Qt::white);
    painter.fillPath(innerCircle, RingTheme::presenceGreen_);
}
