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

#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QMenuBar>
#include "windowbarupone.h"


WindowBarUpOne::WindowBarUpOne( QToolBar* parent )
: QToolBar(parent)
, spacer_(new QWidget())
, dl( QPoint() )
{
  setFixedHeight( 30 );
  
  setMovable( false );
  
  spacer_->setSizePolicy( QSizePolicy::Expanding,QSizePolicy::Expanding );
  
  addWidget( spacer_ );
  
  minimize = addAction( QIcon( ":images/minimize-ring.png" ), "minimize" );
  
  maximize = addAction( QIcon( ":images/maximize-ring.png" ), "maximize" );
  
  quit = addAction( QIcon( ":images/close-ring.png" ), "quit" );
  
}


WindowBarUpOne::~WindowBarUpOne(  )
{
	delete spacer_;

}

        
void
WindowBarUpOne::paintEvent( QPaintEvent* event )
{
	Q_UNUSED( event );
	
	QPainter painter( this );
  
	QBrush background( QColor( 63, 63, 63 ) );

	painter.setBrush( background );
	painter.setPen( Qt::NoPen ); // No stroke
	painter.drawRect( 0, 0, width(), height() );
	
}


void
WindowBarUpOne::mousePressEvent( QMouseEvent* event )
{
	dl = event->pos();
  
	setCursor( QCursor( Qt::SizeAllCursor ) );
	
}


void
WindowBarUpOne::mouseReleaseEvent( QMouseEvent* event )
{
	Q_UNUSED( event );
  
	setCursor( QCursor( Qt::ArrowCursor ) );
}


void
WindowBarUpOne::mouseMoveEvent( QMouseEvent* event )
{
	QPoint p = event->globalPos();
  
	window()->move( p - dl );
	
}
