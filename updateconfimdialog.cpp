/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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
#include "ui_updateconfirm.h"
#include "updateconfimdialog.h"

UpdateConfimDialog::UpdateConfimDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::UpdateConfimDialog)
{
    ui->setupUi(this);

    connect(ui->btnUpdateAccept, &QPushButton::clicked, this, &UpdateConfimDialog::on_updateAcceptBtn_clicked);
    connect(ui->btnUpdateCancel, &QPushButton::clicked, this, &UpdateConfimDialog::on_updateCancelBtn_clicked);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UpdateConfimDialog::~UpdateConfimDialog()
{
    delete ui;
}

void UpdateConfimDialog::on_updateCancelBtn_clicked()
{
    done(DialogCode::Rejected);
}

void UpdateConfimDialog::on_updateAcceptBtn_clicked()
{
    done(DialogCode::Accepted);
}
