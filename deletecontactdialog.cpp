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

#include "deletecontactdialog.h"
#include "ui_deletecontactdialog.h"

// LRC
#include "person.h"
#include "account.h"
#include "bannedcontactmodel.h"

DeleteContactDialog::DeleteContactDialog(ContactMethod* cm, Account* ac, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteContactDialog),
    contactMethod_(cm),
    account_(ac)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
    ui->contactNameLabel->setText(cm->bestName());
    if (cm->bestName() != cm->bestId())
        ui->contactIdLabel->setText(cm->bestId());
    else
        ui->contactIdLabel->hide();
}

DeleteContactDialog::~DeleteContactDialog()
{
    delete ui;
}

void DeleteContactDialog::on_deleteCancelBtn_clicked()
{
    close();
}

void DeleteContactDialog::on_deleteAcceptBtn_clicked()
{
    account_->removeContact(contactMethod_);
    accept();
}

void DeleteContactDialog::on_deleteBanBtn_clicked()
{
    account_->removeContact(contactMethod_);
    account_->bannedContactModel()->add(contactMethod_);
    accept();
}
