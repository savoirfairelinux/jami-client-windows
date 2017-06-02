/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "quickactcontactrequestwidget.h"
#include "ui_quickactcontactrequestwidget.h"

#include <QFont>

// CLIENT
#include "contactrequestitemdelegate.h"

QuickActContactRequestWidget::QuickActContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QuickActContactRequestWidget)
{
    ui->setupUi(this);

    // set symbols in buttons using FontAwsome
    ui->quickValidCRBtn->setText(QChar(0xf00c));
    ui->quickMuteCRBtn->setText(QChar(0xf12d));
    ui->quickBanCRBtn->setText(QChar(0xf00d));

    connect(ui->quickValidCRBtn, &QPushButton::clicked, this, [=](){
        emit quickValidCRBtnClicked();
    });

    connect(ui->quickMuteCRBtn, &QPushButton::clicked, this, [=](){
        emit quickMuteCRBtnClicked();
    });

    connect(ui->quickBanCRBtn, &QPushButton::clicked, this, [=](){
        emit quickBanCRBtnClicked();
    });

}

QuickActContactRequestWidget::~QuickActContactRequestWidget()
{
    disconnect(this);
    delete ui;
}
