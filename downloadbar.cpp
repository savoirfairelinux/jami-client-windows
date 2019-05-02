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

#include "downloadbar.h"
#include "ui_downloadbar.h"

updateDownloadDialog::updateDownloadDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::updateDownloadDialog)
{
    ui->setupUi(this);
    setStyleSheet("background: rgb(255, 255, 255)");
    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void updateDownloadDialog::setValue(double var)
{
    value_ = var;
}

void updateDownloadDialog::setMaximum(double var)
{
    maximum_ = var;
}

updateDownloadDialog::~updateDownloadDialog()
{
    delete ui;
}

void updateDownloadDialog::update(QString displayText)
{
    int percent = static_cast<int>((1 - ((maximum_ - value_) / maximum_)) * 100);
    ui->Statusedit->setText("Speed: " + displayText);
    ui->progressBar->setValue(percent);
}
