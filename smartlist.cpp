/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Jäger Nicolas <nicolas.jager@savoirfairelinux.com>              *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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
#include <QEvent>
#include <QTreeWidgetItem>
#include <QScrollBar>

#include "smartlistdelegate.h"
#include "combar.h"
#include "smartlist.h"

SmartList::SmartList(QWidget *parent) :
    QTreeView(parent)
{
    verticalScrollBar()->hide();

    connect(this, &QAbstractItemView::entered, [this](const QModelIndex & index) {
        auto widget = indexWidget(index);
        if (!widget) {
            ComBar* bar = new ComBar();
            setIndexWidget(index, bar);
            connect(bar, &ComBar::btnVideoClicked, this, [=](){ emit btnVideoClicked(); });
        }
        else if (index.isValid())
            indexWidget(index)->setVisible(true);

        if(hoveredRow_.isValid() and indexWidget(hoveredRow_))
            indexWidget(hoveredRow_)->setVisible(false);

        hoveredRow_ = index;
    });

    setVerticalScrollMode(ScrollPerPixel);
}

SmartList::~SmartList()
{
    reset();
}

void
SmartList::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    verticalScrollBar()->show();
}

void
SmartList::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);

    hoveredRow_ = QModelIndex();
    verticalScrollBar()->hide();
}

void
SmartList::setSmartListItemDelegate(SmartListDelegate* delegate)
{
    if (delegate) {
        setItemDelegate(delegate);
        smartListDelegate_ = delegate;
    }
}

bool
SmartList::eventFilter(QObject* watched, QEvent* event)
{

    if (qobject_cast<QScrollBar*>(watched) && event->type() == QEvent::Enter) {
        hoveredRow_ = QModelIndex();
        return true;
    }

    return QObject::eventFilter(watched, event);
}


void
SmartList::drawRow(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if(index == hoveredRow_ && indexWidget(hoveredRow_))
        indexWidget(index)->setVisible(true);
    else if(indexWidget(index))
        indexWidget(index)->setVisible(false);

    QTreeView::drawRow(painter, option, index);
}
