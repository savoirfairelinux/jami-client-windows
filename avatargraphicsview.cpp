/**************************************************************************
* Copyright (C) 2018 by Savoir-faire Linux                           *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
* along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
**************************************************************************/

#include "avatargraphicsview.h"

AvatarGraphicsView::AvatarGraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void
AvatarGraphicsView::wheelEvent(QWheelEvent* event)
{
    QPoint numSteps = event->angleDelta() / (8*15);

    if (!numSteps.isNull()) {
        zoomImage(numSteps);
    }
    event->accept();
}

void
AvatarGraphicsView::zoomImage(const QPoint& steps)
{
    steps.y() == 1 ? scale(zoomIncrement_, zoomIncrement_) : scale(1 / zoomIncrement_, 1 / zoomIncrement_);
}

void
AvatarGraphicsView::paintEvent(QPaintEvent* e)
{
    QGraphicsView::paintEvent(e);
    QPainter painter(viewport());

    QPainterPath square;
    QPainterPath circle;

    square.addRect(0, 0, width(), height());
    circle.addEllipse(QPoint(width() / 2 - 0.5, height() / 2 - 0.5), circleSize_ + 1, circleSize_ + 1);

    painter.setOpacity(0.3);

    painter.fillPath(square.subtracted(circle), QBrush(QColor(Qt::black)));
}