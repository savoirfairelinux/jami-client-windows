/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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

#include "contactrequestwidget.h"
#include "ui_contactrequestwidget.h"

ContactRequestWidget::ContactRequestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ContactRequestWidget)
{
    ui->setupUi(this);
}

ContactRequestWidget::~ContactRequestWidget()
{
    delete ui;
}

void
ContactRequestWidget::setCurrentContactRequest(ContactRequest *cr)
{
    cr_ = cr;
    if (cr_ != nullptr) {
        QString remoteId = QString::fromLocal8Bit(cr_->roleData(Qt::DisplayRole).value<QByteArray>());
        ui->remoteIdLabel->setText(QString("Current ContactRequest: %1").arg(remoteId));
    }
}

void
ContactRequestWidget::on_acceptCRButton_clicked()
{
    if (cr_ != nullptr) {
        cr_->accept();
    }
    setCurrentContactRequest(nullptr);
    emit choiceMade();
}

void ContactRequestWidget::on_discardCRButton_clicked()
{
    if (cr_ != nullptr) {
        cr_->discard();
    }
    setCurrentContactRequest(nullptr);
    emit choiceMade();
}

void ContactRequestWidget::on_blockCRButton_clicked()
{
    if (cr_ != nullptr) {
        cr_->block();
    }
    setCurrentContactRequest(nullptr);
    emit choiceMade();
}
