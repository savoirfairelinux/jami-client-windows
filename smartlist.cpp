/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                           *
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
            removeCombar();
            if (auto widget = indexWidget(index)) {
                widget->setVisible(true);
            } else {
                ComBar* bar = new ComBar();
                setIndexWidget(index, bar);
                connect(bar, &ComBar::btnVideoClicked, this, [=](){ emit btnVideoClicked(); });
            }
            hoveredRow_ = index;
    });

    setVerticalScrollMode(ScrollPerPixel);
}

SmartList::~SmartList()
{

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

    verticalScrollBar()->hide();

    removeCombar();
}

void
SmartList::setSmartListItemDelegate(SmartListDelegate* delegate)
{
    if (delegate)
    {
        setItemDelegate(delegate);
        smartListDelegate_ = delegate;
    }
}

bool
SmartList::eventFilter(QObject* watched, QEvent* event)
{

    if (qobject_cast<QScrollBar*>(watched) && event->type() == QEvent::Enter) {
        removeCombar();
        return true;
    }

    return QObject::eventFilter(watched, event);
}

void
SmartList::removeCombar()
{
    if(not hoveredRow_.isValid())
        return;

    if (auto widget = indexWidget(hoveredRow_)) {
        widget->setVisible(false);
    }
}

void
SmartList::resizeEvent(QResizeEvent* event)
{
    reset();
    QTreeView::resizeEvent(event);
}
