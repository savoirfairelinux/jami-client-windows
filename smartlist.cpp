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
#include "smartlist.h"

// TODO renames to something like smartlist

SmartList::SmartList( QWidget *parent )
: currentRow(-1)
, comBar_(new ComBar(this))
{
	setMouseTracking(true);
	
	setHeaderHidden(true);
	
	setStyleSheet(
	"QScrollBar:vertical { background:white; width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 77, 77, 77 ) }"
	"RingTreeView{background-color : white; color}"
	);
}


SmartList::~SmartList(  )
{
	delete comBar_;
}


void
SmartList::leaveEvent( QEvent * event )
{
	currentRow = -1;	
	
    if ( smartListDelegate_ )
    {
		smartListDelegate_->setRowHighlighted( currentRow );
	}
	
	setStyleSheet(
	"QScrollBar:vertical { background:white; width:10px; }"
    "QScrollBar::handle:vertical { background: rgb( 77, 77, 77 ) }"
	"RingTreeView{background-color : white; color}"
	);
	
}


void
SmartList::paintEvent( QPaintEvent* event )
{
    QTreeView::paintEvent( event );
    
    // NICOO RM ME
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
    
    currentRow = index.row();
    
    if ( smartListDelegate_ )
    {
		smartListDelegate_->setRowHighlighted( currentRow );
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
	
		connect( smartListDelegate_ , &SmartListDelegate::MonSignal , comBar_, &ComBar::MonSlot );
	
	}
	
}
