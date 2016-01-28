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
#include <qevent.h>
#include <QTreeWidgetItem>
#include <QScrollBar>
#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "smartlistdelegate.h"
#include "combar.h"
#include "smartlist.h"
#include "ui_combar.h"

SmartList::SmartList(QWidget *parent) :
    QTreeView(parent)
{

    verticalScrollBar()->hide();

    connect(this, &QAbstractItemView::entered, [=](const QModelIndex & index) {
            removeTheCombar(hoveredRow_);
            ComBar* barbar = new ComBar();
            setIndexWidget(index, barbar);
            hoveredRow_ = index;
            connect( barbar, ComBar::btnvideoClicked, this, [=](){
                emit btnvideoClicked();
            });
    });

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

    removeTheCombar(hoveredRow_);
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

    QMouseEvent *e = dynamic_cast<QMouseEvent *> (event);

    if ( qobject_cast<QScrollBar*>(watched)
                                     && event->type() == QEvent::Enter )
    {
        removeTheCombar(hoveredRow_);
        return true;
    }

    return QObject::eventFilter(watched, event);
}

void
SmartList::removeTheCombar(const QModelIndex & index)
{
    if( not index.isValid() )
        return;

    QWidget* blankWidget = new QWidget();
    blankWidget->setAttribute(Qt::WA_TransparentForMouseEvents );
    setIndexWidget(index, blankWidget );
}
