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
#include "ui_updateconfirmdialog.h"
#include "updateconfirmdialog.h"

UpdateConfirmDialog::UpdateConfirmDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::UpdateConfirmDialog)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

UpdateConfirmDialog::~UpdateConfirmDialog()
{
    delete ui;
}

void
UpdateConfirmDialog::changeToUpdateToBetaVersionText()
{
    //setSizePolicy();
    //setMaximumHeight();
    auto originHeight = ui->labelWarning->height();
    ui->labelDeletion->setText("Install the newest Beta version?");
    ui->labelWarning->setText("This will uninstall your current Jami, and it is not revertable.\n\
                                Be awared of the potential issues that may exist in Beta Version.\n\
                                You can always download the newest Release version on our website.");
    //auto currentHeight =
   //setMaximumHeight(height() + )
}

void
UpdateConfirmDialog::on_updateCancelBtn_clicked()
{
    done(DialogCode::Rejected);
}

void
UpdateConfirmDialog::on_updateAcceptBtn_clicked()
{
    done(DialogCode::Accepted);
}
