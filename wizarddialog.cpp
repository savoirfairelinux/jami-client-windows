/***************************************************************************
 * Copyright (C) 2015-2016 by Savoir-faire Linux                                *
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

#include <QMovie>

#include "accountmodel.h"
#include "account.h"

#include "utils.h"

WizardDialog::WizardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);

    QPixmap logo(":/images/logo-ring-standard-coul.png");
    ui->ringLogo->setPixmap(logo.scaledToHeight(65, Qt::SmoothTransformation));
    ui->ringLogo->setAlignment(Qt::AlignHCenter);

    ui->usernameEdit->setText(Utils::GetCurrentUserName());
}

WizardDialog::~WizardDialog()
{
    delete ui;
}

void
WizardDialog::accept()
{
    ui->label->setText(tr("Please wait while we create your account."));
    ui->wizardButton->setEnabled(false);
    ui->usernameEdit->setEnabled(false);

    repaint();

    Utils::CreateStartupLink();

    auto account = AccountModel::instance().add(ui->usernameEdit->text(), Account::Protocol::RING);
    if (not ui->usernameEdit->text().isEmpty())
        account->setDisplayName(ui->usernameEdit->text());
    else
        account->setDisplayName(tr("Unknown"));
    AccountModel::instance().ip2ip()->setRingtonePath(Utils::GetRingtonePath());
    account->setRingtonePath(Utils::GetRingtonePath());
    account->setUpnpEnabled(true);

    connect(account, SIGNAL(changed(Account*)), this, SLOT(endSetup(Account*)));

    account->performAction(Account::EditAction::SAVE);
}

void
WizardDialog::endSetup(Account* a)
{
    Q_UNUSED(a)
    QDialog::accept();
}

void
WizardDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    exit(0);
}
