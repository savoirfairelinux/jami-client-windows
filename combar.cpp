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

#include "combar.h"
#include "ui_combar.h"

ComBar::ComBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ComBar)
{
    ui->setupUi(this);

    // [jn] these buttons are for further uses
    ui->btnchat->hide();
    ui->btncontactinfo->hide();
}

ComBar::~ComBar()
{
    delete ui;
}

void
ComBar::moveToRow(const QRect& rect)
{
    move(rect.right() - width(),
         rect.bottom() - height() - (rect.height()/4));
}
