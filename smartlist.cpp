/***************************************************************************
 * Copyright (C) 2015 by Savoir-faire Linux                                *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
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

#include <QStyledItemDelegate>
#include <qevent.h>
#include <QTreeWidgetItem>

#include "smartlistdelegate.h"
#include "combar.h"
#include "smartlist.h"
#include "smartlistscrollbar.h"

SmartList::SmartList(QWidget *parent) :
    QTreeView(parent),
    comBar_(new ComBar(this)),
    smartListScrollBar_(new SmartListScrollBar(this))
{
    comBar_->hide();

    setVerticalScrollBar(smartListScrollBar_);
    connect(smartListScrollBar_
           , &SmartListScrollBar::enterSignal
           , [=]()
             {
                 comBar_->hide();
                 smartListDelegate_->setHoveredRow(QModelIndex());
                 repaint();
             });
}

SmartList::~SmartList()
{
    delete comBar_;
}

void
SmartList::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
}

void
SmartList::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);

    if (smartListDelegate_)
        smartListDelegate_->setHoveredRow(QModelIndex());

    comBar_->hide();
    repaint();

}

void
SmartList::wheelEvent(QWheelEvent* event)
{
    comBar_->hide();

    if (smartListDelegate_)
        smartListDelegate_->setHoveredRow(QModelIndex());

    QTreeView::wheelEvent(event);
}

void
SmartList::paintEvent(QPaintEvent* event)
{
    event = new QPaintEvent(QRect(0, 0, width(), height()));
    QPoint p = this->mapFromGlobal(QCursor::pos());
    QModelIndex index = indexAt(p);

    if (smartListDelegate_ && index.isValid())
    {
        smartListDelegate_->setHoveredRow(index);
        comBar_->show();
    }

    QTreeView::paintEvent(event);
}

void
SmartList::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    QModelIndex index = indexAt(event->pos());

    if (smartListDelegate_ && index.isValid())
    {
        smartListDelegate_->setHoveredRow(index);
        comBar_->show();
    }
}

void
SmartList::setSmartListItemDelegate(SmartListDelegate* delegate)
{
    if (delegate)
    {
        setItemDelegate(delegate);
        smartListDelegate_ = delegate;
        connect(smartListDelegate_ , &SmartListDelegate::rowHovering , comBar_, &ComBar::moveToRow);
    }
}