/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include "conversationfilterbutton.h"
#include <QPainter>

ConversationFilterButton::ConversationFilterButton()
{
    setCheckable(false);
    connect(this, &ConversationFilterButton::pressed, this, &ConversationFilterButton::setSelected);
}


ConversationFilterButton::~ConversationFilterButton()
{
}

void
ConversationFilterButton::paintEvent(QPaintEvent * e)
{
    QPushButton::paintEvent(e);
    QPainter painter(this);

    if (selected_) {
        this->setStyleSheet("background-color: rgb(250, 250, 250)");
    } else {
        this->setStyleSheet("background-color: rgb(237, 237, 237)");
    }
    painter.end();
}

void
ConversationFilterButton::setSelected()
{
    selected_ = true;
}

void
ConversationFilterButton::setUnselected()
{
    selected_ = false;
    update();
}

