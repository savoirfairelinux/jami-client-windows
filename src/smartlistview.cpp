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

#include "smartlistmodel.h"
#include "utils.h"

#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>

#include <ciso646>

SmartListView::SmartListView(QWidget *parent) :
    QTreeView(parent)
{
    verticalScrollBar()->setEnabled(true);
    setVerticalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 0px; }");
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setMouseTracking(true);
    installEventFilter(this);

    QObject::connect(this, &QAbstractItemView::entered,
        [this](const QModelIndex & index) {
            auto type = Utils::toEnum<lrc::api::profile::Type>(
                index.data(static_cast<int>(SmartListModel::Role::ContactType)).value<int>()
                );
        });
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
    hoveredRow_ = QModelIndex();
}

void
SmartListView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        emit customContextMenuRequested(event->pos());
    }
    else {
        QTreeView::mousePressEvent(event);
    }
}

bool
SmartListView::eventFilter(QObject* watched, QEvent* event)
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

void
SmartListView::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (index == hoveredRow_ && indexWidget(hoveredRow_)) {
        indexWidget(index)->setVisible(true);
    } else if (indexWidget(index)) {
        auto type = Utils::toEnum<lrc::api::profile::Type>(
            index.data(static_cast<int>(SmartListModel::Role::ContactType)).value<int>()
            );
        if (type == lrc::api::profile::Type::PENDING) {
            indexWidget(index)->setVisible(false);
        }
    }
    QTreeView::drawRow(painter, option, index);
}