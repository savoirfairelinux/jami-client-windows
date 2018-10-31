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
#include "messagemodel.h"

static int created = 0;
static int deleted = 0;

MessageListView::MessageListView(QWidget *parent) :
    QTreeView(parent)
{
    verticalScrollBar()->setEnabled(true);
    setVerticalScrollMode(ScrollPerPixel);
    verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 0px; }");
    setDragEnabled(false);
    setAcceptDrops(false);
    setDropIndicatorShown(false);
    setSelectionMode(QAbstractItemView::NoSelection);
    setMouseTracking(true);
    installEventFilter(this);

    QObject::connect(this, &QAbstractItemView::entered,
        [this](const QModelIndex & index) {
            if (!index.isValid()) {
                return;
            }
            auto type = static_cast<lrc::api::interaction::Type>(index
                .data(static_cast<int>(MessageModel::Role::Type))
                .value<int>());
            if (Utils::isInteractionGenerated(type)) {
                return;
            }
            removeMsgWidgets();
            auto widget = indexWidget(index);
            if (!widget) {
                SelectableMessageWidget* msgWidget = new SelectableMessageWidget(index, this);
                qDebug() << "created: " << ++created;
                disposeBag_.emplace_back(index.row());
                setIndexWidget(index, msgWidget);
                if (auto messageModel = qobject_cast<MessageModel*>(this->model())) {
                    messageModel->disposeBag_.emplace_back(index.row());
                }
                connect(msgWidget, &SelectableMessageWidget::exited, this,
                    [this, index]() {
                        removeMsgWidget(index);
                    });
                connect(msgWidget, &SelectableMessageWidget::entered, this,
                    [this, index]() {
                        if (auto msgWidget = qobject_cast<SelectableMessageWidget*>(indexWidget(index))) {
                            msgWidget->makeFreeable();
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
}

bool
MessageListView::eventFilter(QObject* watched, QEvent* event)
{
    auto index = this->indexAt(static_cast<QMouseEvent*>(event)->pos());
    if (!index.isValid()) {
        removeMsgWidgets();
    }
    if (qobject_cast<QScrollBar*>(watched) && event->type() == QEvent::Enter) {
        removeMsgWidgets();
        return true;
    }
    return QObject::eventFilter(watched, event);
}

void
MessageListView::removeMsgWidgets()
{
    auto messageModel = qobject_cast<MessageModel*>(this->model());
    if (!messageModel) {
        return;
    }
    for (auto row : messageModel->disposeBag_) {
        removeMsgWidget(this->model()->index(row, 0));
    }
}

void
MessageListView::removeMsgWidgetTracking(int row)
{
    auto messageModel = qobject_cast<MessageModel*>(this->model());
    if (!messageModel) {
        return;
    }
    auto& disposeBag = messageModel->disposeBag_;
    auto indexToErase = Utils::indexInVector<int>(disposeBag, row);
    if (messageModel && indexToErase < disposeBag.size()) {
        disposeBag.erase(disposeBag.begin() + indexToErase);
    }
}

void
MessageListView::removeMsgWidget(const QModelIndex& index)
{
    auto widget = qobject_cast<SelectableMessageWidget*>(indexWidget(index));
    if (widget && widget->isFreeable()) {
        setIndexWidget(index, nullptr);
        widget->setParent(nullptr);
        widget->deleteLater();
        connect(widget, &QObject::destroyed, this,
            [this, row = index.row()]() {
                qDebug() << "destroyed: " << ++deleted;
                removeMsgWidgetTracking(row);
            });
    }
}