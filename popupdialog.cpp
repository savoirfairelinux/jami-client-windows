/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "popupdialog.h"
#include "ui_popupdialog.h"

#include "utils.h"

#include <QMouseEvent>

PopupDialog::PopupDialog(QWidget *parent,
                         QColor spikeColor,
                         SpikeLabelAlignment spikeAlignment) :
    QDialog(parent),
    ui(new Ui::PopupDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setSpikeLabelAlignment(spikeAlignment);
    ui->spikeLabel->setPixmap(Utils::generateTintedPixmap(":/images/spike.png", spikeColor));
}

PopupDialog::~PopupDialog()
{
    delete ui;
}

void
PopupDialog::setSpikeLabelAlignment(SpikeLabelAlignment spikeAlignment)
{
    switch (spikeAlignment)
    {
    case PopupDialog::SpikeLabelAlignment::AlignLeft:
        ui->spikeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        break;
    case PopupDialog::SpikeLabelAlignment::AlignHCenter:
        ui->spikeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
        break;
    case PopupDialog::SpikeLabelAlignment::AlignRight:
        ui->spikeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        break;
    default:
        break;
    }
}

void
PopupDialog::insertWidget(QWidget * widget)
{
    ui->containerWidget->layout()->addWidget(widget);
}

int PopupDialog::getSpikeHeight()
{
    return ui->spikeLabel->height();
}

void
PopupDialog::mousePressEvent(QMouseEvent* event)
{
    auto dialogRect = rect();
    if (!dialogRect.contains(event->pos())) {
        emit willClose(event);
    }
    QDialog::mousePressEvent(event);
}
