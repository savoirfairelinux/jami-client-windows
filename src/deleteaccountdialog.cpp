/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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

#include "lrcinstance.h"
#include "utils.h"

DeleteAccountDialog::DeleteAccountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteAccountDialog)
{
    ui->setupUi(this);
    if (LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::RING){
        ui->labelBestId->setText(Utils::bestNameForAccount(LRCInstance::getCurrentAccountInfo()));
        ui->labelAccountHash->setText(LRCInstance::getCurrentAccountInfo().profileInfo.uri);
    } else {
        ui->labelWarning->hide();
    }
    connect(ui->btnDeleteAccept, &QPushButton::clicked, this, &DeleteAccountDialog::on_deleteAcceptBtn_clicked);
    connect(ui->btnDeleteCancel, &QPushButton::clicked, this, &DeleteAccountDialog::on_deleteCancelBtn_clicked);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setFixedSize(size());
}

DeleteAccountDialog::~DeleteAccountDialog()
{
    delete ui;
}

void
DeleteAccountDialog::on_deleteCancelBtn_clicked()
{
    done(DialogCode::Rejected);
}

void
DeleteAccountDialog::on_deleteAcceptBtn_clicked()
{
    LRCInstance::accountModel().removeAccount(LRCInstance::getCurrAccId());
    done(DialogCode::Accepted);
}
