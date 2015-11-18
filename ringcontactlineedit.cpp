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
 
#include <QtWidgets/QApplication>
#include "ringcontactlineedit.h"
#include <QDebug>
#include <QIcon>



RingContactLineEdit::RingContactLineEdit(QWidget *parent) :
	QLineEdit(parent)
{
	QPalette palette;
	palette.setColor( QPalette::Base, QColor(242,242,242) );
	//~ palette.setColor(QPalette::Text,Qt::white);
	setPalette(palette);

    setFrame(false);
    
    setStyleSheet("RingContactLineEdit {"
		"border-color: rgb(242, 242, 242);"
		"border-radius: 5px;"
		"border-width: 2px;"
        "background-color: rgb(242, 242, 242) }");
        
    addAction(QIcon(":/images/search-contact.png"), QLineEdit::ActionPosition::LeadingPosition);
		
		
		    //~ background-color: red;
    //~ border-style: outset;
    //~ border-width: 2px;
    //~ border-radius: 10px;
    //~ border-color: beige;
		
		
}

RingContactLineEdit::~RingContactLineEdit()
{
}

void RingContactLineEdit::paintEvent(QPaintEvent * event)
{
    QLineEdit::paintEvent(event);
    
}
