/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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

#include "bezierconnectorwidget.h"

#include <QPainter>
#include <QDebug>
#include <QPainterPath>

BezierConnectorWidget::BezierConnectorWidget(QWidget* parent) : QWidget(parent)
{
}

BezierConnectorWidget::~BezierConnectorWidget()
{
}

void
BezierConnectorWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing, true);

    QPen pen(painter.pen());
    pen.setWidth(2.0);
    pen.setColor(QColor(240, 240, 240));
    painter.setPen(pen);

    QPainterPath bezierPath;
    double c = 0.551915024494;
    QPointF p0, p1, p2, p3;

    if (this->objectName().contains("right", Qt::CaseInsensitive)) {
        p0 = rect().topLeft();
        p3 = rect().bottomRight();
        p1 = QPointF(p0.x() + c * rect().width(), p0.y());
        p2 = QPointF(p0.x() + rect().width(), p0.y() + rect().height() - (c * rect().height()));
    }
    else if (this->objectName().contains("left", Qt::CaseInsensitive)) {
        p0 = rect().bottomLeft();
        p3 = rect().topRight();
        p1 = QPointF(p0.x(), p3.y() + c * rect().height());
        p2 = QPointF(p0.x() + c * rect().width(), p3.y());
    }

    bezierPath.moveTo(p0);
    bezierPath.cubicTo(p1, p2, p3);
    painter.drawPath(bezierPath);
}
