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
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QMenuBar>
#include "windowbaruptwo.h"

WindowBarUpTwo::WindowBarUpTwo(QToolBar* parent) :
    QToolBar(parent),
    spacer_(new QWidget()),
    dl( QPoint() )
{
  setFixedHeight(25);

  setMovable(false);

  contactslist_ = addAction( QIcon(":images/contactswidgetlist.png"), tr("contacts") );

  contactslist_->setCheckable(true);

  history_ = addAction( QIcon(":images/historylist.png"), tr("history") );

  history_->setCheckable(true);

  spacer_->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  addWidget(spacer_);

  settings_ = addAction( QIcon(":images/settings.png"), tr("settings") );

  // further use
  //~ info_ = addAction( QIcon(":images/btn-info.svg"), tr("info") );

  connect( history_, &QAction::triggered, this, &WindowBarUpTwo::historyToggled );

  connect( contactslist_, &QAction::triggered, this, &WindowBarUpTwo::contactslistToggled );

}

WindowBarUpTwo::~WindowBarUpTwo()
{
    delete spacer_;

}

void
WindowBarUpTwo::paintEvent(QPaintEvent* event)
{
    Q_UNUSED( event );

    QPainter painter(this);

    QBrush background(QColor(63, 63, 63));

    painter.setBrush(background);
    painter.setPen  (Qt::NoPen ); // No stroke

    painter.drawRect(0, 0, width(), height());
}


void
WindowBarUpTwo::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED( event );

    // event->pos() return the position inside the current widget,
    // so I`m adding the height of the RingWindowBar to get the good
    // computation when the mouse is moved.
    dl = event->pos();
    dl+= QPoint(0, 30);

}

void
WindowBarUpTwo::historyToggled(bool checked)
{
    if ( checked )
    {
        contactslist_->setChecked(false);
        history_->setChecked(true);
    }
    else
    {
        history_->setChecked(false);
    }
}

void
WindowBarUpTwo::contactslistToggled(bool checked)
{
    if ( checked )
    {
        history_->setChecked(false);
        contactslist_->setChecked(true);
    }
    else
    {
        contactslist_->setChecked(false);
    }
}

void
WindowBarUpTwo::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

}

void
WindowBarUpTwo::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    QPoint p = event->globalPos();

    window()->move(p - dl);

}
