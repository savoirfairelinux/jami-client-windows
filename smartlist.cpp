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
#include <QScrollBar>
#include <QApplication>

#include "smartlistdelegate.h"
#include "combar.h"
#include "smartlist.h"

SmartList::SmartList(QWidget *parent) :
    QTreeView(parent),
    comBar_(new ComBar(this))
{
    verticalScrollBar()->installEventFilter(this);

    comBar_->installEventFilter(this);

    connect(this, &QTreeView::entered, this, &SmartList::update );
}

SmartList::~SmartList()
{
    delete comBar_;
}

void
SmartList::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    update();
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

    if ( qobject_cast<QScrollBar*>(watched)
                                     && event->type() == QEvent::Enter )
    {
        update();
        return true;

    }
    else // this one discard some bad user behavior...
    if ( qobject_cast<QScrollBar*>(watched)
                       && event->type() == QEvent::Wheel
                       && QApplication::mouseButtons() != Qt::NoButton )
    {
        return true;
    }
    else
    if ( qobject_cast<ComBar*>(watched) && event->type() == QEvent::Wheel
                       && QApplication::mouseButtons() == Qt::NoButton )
    {
        wheelEvent(static_cast<QWheelEvent*>(event));
        return true;
    }

    return QObject::eventFilter(watched, event);
}

void
SmartList::update(const QModelIndex& index)
{
    if( index.isValid() )
        setCurrentIndex(index);

    QRect rect = visualRect(index);

    comBar_->moveToRow( index, rect );

    smartListDelegate_->setHoveredRow((index.isValid())?
                                                 index : QModelIndex());
    repaint();

}
