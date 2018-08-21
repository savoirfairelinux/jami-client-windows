/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "smartlistview.h"

#include <QScrollBar>
#include <QMouseEvent>

#include <ciso646>

SmartListView::SmartListView(QWidget *parent) :
    QListView(parent)
{
    verticalScrollBar()->setEnabled(true);
    setVerticalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 0px; }");

    // TODO: visible overlay scrollbar

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
}

SmartListView::~SmartListView()
{
    reset();
}

void
SmartListView::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
}

void
SmartListView::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
}

void
SmartListView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        emit customContextMenuRequested(event->pos());
    } else {
        QListView::mousePressEvent(event);
    }
}
