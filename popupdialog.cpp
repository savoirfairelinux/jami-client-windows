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
#include <QPainter>

PopupDialog::PopupDialog(QWidget *parent,
                         QColor spikeColor,
                         SpikeLabelAlignment spikeAlignment) :
    QDialog(parent),
    ui(new Ui::PopupDialog),
    spikeColor_(spikeColor)
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
    case PopupDialog::SpikeLabelAlignment::None:
        ui->spikeLabel->hide();
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

void
PopupDialog::mousePressEvent(QMouseEvent* event)
{
    auto dialogRect = rect();
    if (!dialogRect.contains(event->pos())) {
        emit willClose(event);
    }
    QDialog::mousePressEvent(event);
}

void
PopupDialog::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // draw rounded corner
    Utils::fillRoundRectPath(painter, spikeColor_, ui->containerWidget->rect(), cornerRadius_);
    QDialog::paintEvent(event);
}
