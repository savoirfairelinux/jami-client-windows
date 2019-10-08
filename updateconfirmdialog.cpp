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

#include <QFontMetrics>

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

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    setMaximumHeight(INT_MAX);
    setMaximumWidth(INT_MAX);

    QString warning { QString("1. This will uninstall your current Jami, and it is not revertable.\n") +
                      QString("2. Be awared of the potential issues that may exist in Beta Version.\n") +
                      QString("3. You can always download the newest Release version on our website.\n") };
    ui->labelWarning->setText(warning);
    auto rect = ui->labelWarning->fontMetrics().boundingRect(ui->labelWarning->rect(), Qt::AlignCenter, warning);
    ui->labelWarning->setMinimumHeight(rect.height() + ui->labelWarning->height());
    ui->labelWarning->setMinimumWidth(rect.width());
    setWindowTitle("Jami Beta Installation");
    ui->labelDeletion->setText("Install the newest Beta version?");

    setMaximumHeight(height() + rect.height());
    setMinimumHeight(height() + rect.height());
    setMaximumWidth(rect.width() + 50);
    setMinimumWidth(rect.width() + 50);
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
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
