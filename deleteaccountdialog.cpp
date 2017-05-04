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

#include "deleteaccountdialog.h"
#include "ui_deleteaccountdialog.h"

// LRC
#include "accountmodel.h"
#include "itemdataroles.h"
#include "account.h"

DeleteAccountDialog::DeleteAccountDialog(const QModelIndex & idx, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteAccountDialog),
    index_(idx)
{
    ui->setupUi(this);
    ui->accountAliasLabel->setText(AccountModel::instance().data(index_, Qt::DisplayRole).toString());
    auto ac = AccountModel::instance().getAccountByModelIndex(index_);
    if (ac->protocol() == Account::Protocol::RING){
        ui->accountIdLabel->setAlignment(Qt::AlignCenter);
        ui->accountIdLabel->setText((ac->registeredName().isEmpty())? ac->username(): ac->registeredName() + "\n" + ac->username());
    } else {
        ui->warningLabel->hide();
    }
}

DeleteAccountDialog::~DeleteAccountDialog()
{
    delete ui;
}

void DeleteAccountDialog::on_deleteCancelBtn_clicked()
{
    close();
}

void DeleteAccountDialog::on_deleteAcceptBtn_clicked()
{
    auto account = AccountModel::instance().getAccountByModelIndex(index_);
    AccountModel::instance().remove(account);
    AccountModel::instance().save();
    close();
}
