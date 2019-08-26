/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
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
#include "ui_sipkeypad.h"
#include "sipinputpanel.h"

#include "lrcinstance.h"

sipInputPanel::sipInputPanel(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SIPInputPanelDialog)
{
    //Adds a mapping so that when map() is signalled from the sender, the signal mapped(id) is emitted.
    signalMapper->setMapping(ui->panelButton_0, 0);
    signalMapper->setMapping(ui->panelButton_1, 1);
    signalMapper->setMapping(ui->panelButton_2, 2);
    signalMapper->setMapping(ui->panelButton_3, 3);
    signalMapper->setMapping(ui->panelButton_4, 4);
    signalMapper->setMapping(ui->panelButton_5, 5);
    signalMapper->setMapping(ui->panelButton_6, 6);
    signalMapper->setMapping(ui->panelButton_7, 7);
    signalMapper->setMapping(ui->panelButton_8, 8);
    signalMapper->setMapping(ui->panelButton_9, 9);
    signalMapper->setMapping(ui->panelButton_hash, 10);
    signalMapper->setMapping(ui->panelButton_star, 11);

    connect(ui->panelButton_0, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_1, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_2, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_3, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_4, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_5, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_6, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_7, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_8, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_9, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_hash, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_star, SIGNAL(clicked()), signalMapper, SLOT(map()));

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(panelButtonClicked(int)));
}

sipInputPanel::~sipInputPanel()
{
    delete ui;
}

void
sipInputPanel::panelButtonClicked(const int& id)
{
    emit sipInputPanelClicked(id);
}

void
sipInputPanel::mousePressEvent(QMouseEvent *event)
{
    /*auto contactPickerWidgetRect = ui->SIPInputPanel->rect();
    if (!contactPickerWidgetRect.contains(event->pos())) {
        emit willClose(event);
    }*/
}
