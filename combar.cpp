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

#include "combar.h"
#include "ui_combar.h"

ComBar::ComBar(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ComBar)
{
    ui->setupUi(this);
    hide();
    //~ setAttribute( Qt::WA_TransparentForMouseEvents );

    // [jn] this button is for some further use
    ui->btncontactinfo->hide();
}

ComBar::~ComBar()
{
    delete ui;
}

void
ComBar::moveToRow(const QModelIndex& index, const QRect& rect)
{
    hoveredRow_ = QPersistentModelIndex(index);

    if(hoveredRow_.isValid())
    {
        move(rect.right() - width(),
                          rect.bottom() - height() - (rect.height()/4));
        show();
    }
    else
    {
        hide();
    }
}
