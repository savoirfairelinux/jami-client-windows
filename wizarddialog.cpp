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
#include "profilemodel.h"
#include "profile.h"

#include "utils.h"
#include "photoboothdialog.h"

WizardDialog::WizardDialog(QWidget* parent) :
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

    auto profile = ProfileModel::instance().selectedProfile();

    repaint();

    Utils::CreateStartupLink();

    auto account = AccountModel::instance().add(ui->usernameEdit->text(), Account::Protocol::RING);
    if (not ui->usernameEdit->text().isEmpty()) {
        account->setDisplayName(ui->usernameEdit->text());
        profile->person()->setFormattedName(ui->usernameEdit->text());
    }
    else {
        profile->person()->setFormattedName(tr("Unknown"));
    }
    account->setRingtonePath(Utils::GetRingtonePath());
    account->setUpnpEnabled(true);

    connect(account, SIGNAL(changed(Account*)), this, SLOT(endSetup(Account*)));

    account->performAction(Account::EditAction::SAVE);

    profile->setAccounts({account});
    profile->save();
}

void
WizardDialog::endSetup(Account* a)
{
    Q_UNUSED(a)
    QDialog::accept();
}

void
WizardDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)

    exit(0);
}

void
WizardDialog::on_avatarButton_clicked()
{
    PhotoBoothDialog dlg;
    dlg.exec();
    if (dlg.result() == QDialog::Accepted) {
        auto image = QImage(dlg.fileName_);
        auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
        ui->avatarButton->setIcon(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarButton->width())));
    }
}
