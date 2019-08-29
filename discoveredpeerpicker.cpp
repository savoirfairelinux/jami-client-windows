/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "discoveredpeerpicker.h"
#include "ui_discoveredpeerpicker.h"

#include <QMouseEvent>

DiscoveredPeerPicker::DiscoveredPeerPicker(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DiscoveredPeerPicker)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

DiscoveredPeerPicker::~DiscoveredPeerPicker()
{
    delete ui;
}

void DiscoveredPeerPicker::mousePressEvent(QMouseEvent* event)
{
    /*auto contactPickerWidgetRect = ui->contactPickerWidget->rect();
    if (!contactPickerWidgetRect.contains(event->pos())) {
        emit willClose(event);
    }*/
}
