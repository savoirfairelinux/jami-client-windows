/***************************************************************************
 * Copyright (C) 2016 by Savoir-faire Linux                                *
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

#include "pathpassworddialog.h"
#include "ui_pathpassworddialog.h"

#include <QFileDialog>

PathPasswordDialog::PathPasswordDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PathPasswordDialog)
{
    ui->setupUi(this);
}

PathPasswordDialog::~PathPasswordDialog()
{
    delete ui;
}

void
PathPasswordDialog::on_okButton_clicked()
{
    accept();
}

void
PathPasswordDialog::on_cancelButton_clicked()
{
    reject();
}

void
PathPasswordDialog::on_pathButtonEdit_clicked()
{
    QFileDialog dlg;
    if (dlg.exec() == QDialog::Accepted) {

    }
}
