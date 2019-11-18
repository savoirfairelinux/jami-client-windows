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

#include <QMouseEvent>

#include "lrcinstance.h"

SIPInputPanelWidget::SIPInputPanelWidget(QWidget* parent)
    : PopupWidget(parent, Qt::white, PopupDialog::SpikeLabelAlignment::AlignLeft)
    , ui(new Ui::SIPInputPanelWidget)
    , signalMapper(new QSignalMapper(this))
{
    ui->setupUi(this);

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
    signalMapper->setMapping(ui->panelButton_A, 12);
    signalMapper->setMapping(ui->panelButton_B, 13);
    signalMapper->setMapping(ui->panelButton_C, 14);
    signalMapper->setMapping(ui->panelButton_D, 15);

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
    connect(ui->panelButton_A, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_B, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_C, SIGNAL(clicked()), signalMapper, SLOT(map()));
    connect(ui->panelButton_D, SIGNAL(clicked()), signalMapper, SLOT(map()));

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(panelButtonClicked(int)));
}

SIPInputPanelWidget::~SIPInputPanelWidget()
{
    delete ui;
}

void
SIPInputPanelWidget::panelButtonClicked(const int& id)
{
    emit sipInputPanelClicked(id);
}
