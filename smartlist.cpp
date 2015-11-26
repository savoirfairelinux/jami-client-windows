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

#include <QDebug>
#include <QStyledItemDelegate>
#include <qevent.h>
#include <QTreeWidgetItem>
#include "smartlistdelegate.h"
#include "combar.h"
#include "smartlistscrollbar.h"
#include "smartlist.h"


SmartList::SmartList( QWidget *parent )
: currentRow(-1)
, comBar_(new ComBar(this))
, smartListScrollBar_(new SmartListScrollBar(this))
{
    setMouseTracking(true);

    setHeaderHidden(true);

    setVerticalScrollBar(smartListScrollBar_);

    connect( smartListScrollBar_
           , &SmartListScrollBar::enterSignal
           , [=]()
             {
                 smartListDelegate_->setRowHighlighted( -1 );
                 currentRow = -1;
             }
           );

    setStyleSheet(
    "QScrollBar:vertical { background:white; width:0px; }"
    "QScrollBar::handle:vertical { background: rgb( 255, 255, 255 ) }"
    "SmartList{background-color : white; border : none;}"
    );
}


SmartList::~SmartList(  )
{
    delete comBar_;
}

void
SmartList::enterEvent( QEvent* event )
{
    setStyleSheet(
    "QScrollBar:vertical { background: rgb( 242, 242, 242 ); width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 77, 77, 77 ) }"
    "SmartList{background-color : white; border : none;}"
    );
}

void
SmartList::leaveEvent( QEvent* event )
{
    smartListDelegate_->setRowHighlighted( -1 );

    currentRow = -1;

    if ( smartListDelegate_ )
    {
        smartListDelegate_->setRowHighlighted( currentRow );
    }

    setStyleSheet(
    "QScrollBar:vertical { background:white; width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 255, 255, 255 ) }"
    "SmartList{background-color : white; border : none;}"
    );

    comBar_->hide();

}

void
SmartList::wheelEvent(QWheelEvent* event)
{
    currentRow = -1;

    comBar_->hide();

    smartListDelegate_->setRowHighlighted( currentRow );

    setStyleSheet(
    "QScrollBar:vertical { background: rgb( 242, 242, 242 ); width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 77, 77, 77 ) }"
    "SmartList{background-color : white; border : 1px;}"
    );


    QTreeView::wheelEvent( event );

}


void
SmartList::paintEvent( QPaintEvent* event )
{
    QTreeView::paintEvent( event );

    if ( currentRow > -1 )
    {
        comBar_->show();
    }
    else
    {
        comBar_->hide();
    }

}


void
SmartList::mouseMoveEvent( QMouseEvent* event )
{
    QModelIndex index = indexAt( event->pos() );

    setStyleSheet(
    "QScrollBar:vertical { background: rgb( 242, 242, 242 ); width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 77, 77, 77 ) }"
    "SmartList{background-color : white; border : none;}"
    );
    
    currentRow = index.row();

    if ( smartListDelegate_ )
    {
        smartListDelegate_->setRowHighlighted( currentRow );
        
        if ( currentRow > -1 )
        {
            comBar_->show();
        }
        else
        {
            comBar_->hide();
        }
    }

    QTreeView::mouseMoveEvent( event );

}


void
SmartList::setSmartListItemDelegate(SmartListDelegate* delegate)
{
    if ( delegate )
    {

        setItemDelegate(delegate);

        smartListDelegate_ = delegate;

        connect( smartListDelegate_ , &SmartListDelegate::RowSelected , comBar_, &ComBar::Move );

    }

}
