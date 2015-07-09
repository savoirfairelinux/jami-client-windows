/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "transferdialog.h"
#include "ui_transferdialog.h"

#include "callmodel.h"
#include "categorizedcontactmodel.h"

TransferDialog::TransferDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransferDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->activeCallsView->setModel(CallModel::instance());
    ui->activeCallsView->setSelectionModel(CallModel::instance()->selectionModel());

    ui->contactsView->setModel(CategorizedContactModel::instance());
    contactDelegate_ = new ContactDelegate();
    ui->contactsView->setItemDelegate(contactDelegate_);
}

TransferDialog::~TransferDialog()
{
    delete contactDelegate_;
    delete ui;
}

void
TransferDialog::on_cancelButton_clicked()
{
    this->close();
}
