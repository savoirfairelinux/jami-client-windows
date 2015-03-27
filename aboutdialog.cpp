/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->setFixedSize(this->width(),this->height());
    ui->creditsWidget->hide();
    ui->clientVersionLabel->setText(QString("<html><head/><body><p><span style=\" font-weight:600;\">Windows Ring v") + VERSION + "</span></p></body></html>");
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_aboutButton_toggled(bool checked)
{
    ui->creditsButton->setChecked(!checked);
    ui->aboutWidget->setVisible(checked);
    ui->creditsWidget->setVisible(!checked);
}

void AboutDialog::on_creditsButton_clicked(bool checked)
{
    ui->aboutButton->setChecked(!checked);
    ui->creditsWidget->setVisible(checked);
    ui->aboutWidget->setVisible(!checked);
}

void AboutDialog::on_toolButton_clicked()
{
    this->close();
}
