/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
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

#include "ui_ringbutton.h"

#include "ringbutton.h"

RingButton::RingButton(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::RingButton)
{
    ui->setupUi(this);
}

RingButton::~RingButton()
{
    disconnect(this);
    delete ui;
}

void
RingButton::setText(const QString& text)
{
    ui->button->setText(text);
    ui->button->resize(ui->button->sizeHint().width(),
                       ui->button->sizeHint().height());
}

void
RingButton::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event)
    emit clicked();
}
