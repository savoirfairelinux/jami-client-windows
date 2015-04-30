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

#include "wizarddialog.h"
#include "ui_wizarddialog.h"

#include "accountmodel.h"

#include "utils.h"

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::CustomizeWindowHint);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(),this->height());
    ui->buttonBox->setEnabled(false);
}

WizardDialog::~WizardDialog()
{
    delete ui;
}

void
WizardDialog::on_buttonBox_accepted()
{
    auto account = AccountModel::instance()->add(ui->usernameEdit->text(), Account::Protocol::RING);
    account->setRingtonePath(Utils::GetRingtonePath());
    AccountModel::instance()->save();
    Utils::CreateStartupLink();
}

void
WizardDialog::on_usernameEdit_textChanged(const QString &arg1)
{
     ui->buttonBox->setEnabled(!arg1.isEmpty());
}
