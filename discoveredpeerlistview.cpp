/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "discoveredpeerlistview.h"

#include "invitebuttonswidget.h"
#include "utils.h"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>

#include <ciso646>

DiscoveredPeerListView::DiscoveredPeerListView(QWidget* parent)
    : QTreeView(parent)
{
    verticalScrollBar()->setEnabled(true);
    setVerticalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 0px; }");
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
	//When the user selects an item, any already-selected item becomes unselected.
	//It is possible for the user to deselect the selected item.
    setSelectionMode(QAbstractItemView::SingleSelection);
    setMouseTracking(true);
    installEventFilter(this);
}

DiscoveredPeerListView::~DiscoveredPeerListView()
{
    reset();
}

void DiscoveredPeerListView::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
}

void DiscoveredPeerListView::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    hoveredRow_ = QModelIndex();
}

void DiscoveredPeerListView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        emit customContextMenuRequested(event->pos());
    } else {
        QTreeView::mousePressEvent(event);
    }
}

bool DiscoveredPeerListView::eventFilter(QObject* watched, QEvent* event)
{
    auto index = this->indexAt(static_cast<QMouseEvent*>(event)->pos());
    if (!index.isValid()) {
        hoveredRow_ = QModelIndex();
    }
    if (qobject_cast<QScrollBar*>(watched) && event->type() == QEvent::Enter) {
        hoveredRow_ = QModelIndex();
        return true;
    }
    return QObject::eventFilter(watched, event);
}

void DiscoveredPeerListView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QTreeView::drawRow(painter, option, index);
}
