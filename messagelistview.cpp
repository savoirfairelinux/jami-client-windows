/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
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

#include "messagelistview.h"

#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>

#include <ciso646>

#include "utils.h"
#include "selectablemessagewidget.h"

MessageListView::MessageListView(QWidget *parent) :
    QTreeView(parent)
{
    verticalScrollBar()->setEnabled(true);
    setVerticalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 0px; }");
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    setMouseTracking(true);
    installEventFilter(this);

    QObject::connect(this, &QAbstractItemView::entered,
        [this](const QModelIndex & index) {
            if (!index.isValid()) {
                return;
            }
            removeMsgWidgets();
            auto widget = indexWidget(index);
            if (!widget) {
                SelectableMessageWidget* msg = new SelectableMessageWidget(index, this);
                disposeBag_.emplace_back(index.row());
                setIndexWidget(index, msg);
                QMetaObject::Connection* const connection = new QMetaObject::Connection;
                *connection = connect(msg, &SelectableMessageWidget::exited, this,
                    [this, index, connection]() {
                        if (connection) {
                            QObject::disconnect(*connection);
                            delete connection;
                        }
                        if (auto widget = indexWidget(index)) {
                            setIndexWidget(index, nullptr);
                            widget->setParent(nullptr);
                            widget->deleteLater();
                            connect(widget, &QObject::destroyed, this,
                            [this, index]() {
                                auto indexToErase = Utils::indexInVector<int>(disposeBag_, index.row());
                                if (indexToErase < disposeBag_.size()) {
                                    disposeBag_.erase(disposeBag_.begin() + indexToErase);
                                }
                            });
                        }
                    });
            }
        });
}

MessageListView::~MessageListView()
{
    reset();
}

void
MessageListView::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
}

void
MessageListView::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    removeMsgWidgets();
    hoveredRow_ = QModelIndex();
}

bool
MessageListView::eventFilter(QObject* watched, QEvent* event)
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
MessageListView::removeMsgWidgets()
{
    if (!this->model()) {
        return;
    }
    for (auto row : disposeBag_) {
        //auto index = iwPair.first;
        auto index = this->model()->index(row, 0);
        if (auto pWidget = indexWidget(index)) {
            //auto pWidget = iwPair.second;
            setIndexWidget(index, nullptr);
            pWidget->setParent(nullptr);
            pWidget->deleteLater();
            connect(pWidget, &QObject::destroyed, this,
                [this, row]() {
                    auto indexToErase = Utils::indexInVector<int>(disposeBag_, row);
                    if (indexToErase < disposeBag_.size()) {
                        disposeBag_.erase(disposeBag_.begin() + indexToErase);
                    }
                });
        }
    }
}