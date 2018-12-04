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
#pragma once

#include <QGraphicsView>
#include <QWheelEvent>
#include <QDragMoveEvent>


class AvatarGraphicsView : public QGraphicsView
{
    Q_OBJECT
    AvatarGraphicsView(const AvatarGraphicsView& cpy);

public:
    AvatarGraphicsView(QWidget* parent = nullptr);
    inline void setAvatarSize(int size) { circleSize_ = size; };

private:
    void wheelEvent(QWheelEvent* event);
    void zoomImage(const QPoint& steps);

    void paintEvent(QPaintEvent* e);

    const qreal zoomIncrement_ = 1.03;
    int circleSize_ = 160;

    QPainterPath japanPath_;
};