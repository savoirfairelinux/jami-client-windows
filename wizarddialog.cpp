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

    ui->welcomeLogo->setPixmap(logo.scaledToHeight(65, Qt::SmoothTransformation));
    ui->welcomeLogo->setAlignment(Qt::AlignHCenter);

    ui->usernameEdit->setText(Utils::GetCurrentUserName());
}

WizardDialog::~WizardDialog()
{
    delete ui;
}

void
WizardDialog::accept()
{
    if (ui->passwordEdit->text().isEmpty() || ui->passwordEdit->text() != ui->passwordEdit->text()) {
        //TODO: Give some feedback
        return;
    }
    ui->label->setText(tr("Please wait while we create your account."));
    ui->wizardButton->setEnabled(false);
    ui->usernameEdit->setEnabled(false);

    auto profile = ProfileModel::instance().selectedProfile();

    repaint();

    Utils::CreateStartupLink();

    account_ = AccountModel::instance().add(ui->usernameEdit->text(), Account::Protocol::RING);
    if (not ui->usernameEdit->text().isEmpty()) {
        account_->setDisplayName(ui->usernameEdit->text());
        profile->person()->setFormattedName(ui->usernameEdit->text());
    }
    else {
        profile->person()->setFormattedName(tr("Unknown"));
    }
    account_->setRingtonePath(Utils::GetRingtonePath());
    account_->setUpnpEnabled(true);

    account_->setArchivePassword(ui->passwordEdit->text());
    ui->passwordEdit->setEnabled(false);
    ui->confirmPasswordEdit->setEnabled(false);

    if (not ui->pinEdit->text().isEmpty()) {
        account_->setArchivePin(ui->pinEdit->text());
    }

    connect(account_, SIGNAL(stateChanged(Account::RegistrationState)), this, SLOT(endSetup(Account::RegistrationState)));

    account_->performAction(Account::EditAction::SAVE);

    profile->setAccounts({account_});
    profile->save();
}

void
WizardDialog::endSetup(Account::RegistrationState state)
{
    switch (state) {
    case Account::RegistrationState::READY:
    {
        account_->performAction(Account::EditAction::RELOAD);
        QDialog::accept();
        break;
    }
    case Account::RegistrationState::UNREGISTERED:
    case Account::RegistrationState::TRYING:
    case Account::RegistrationState::COUNT__:
        break;
    }
}

void
WizardDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);

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

void
WizardDialog::on_existingPushButton_clicked()
{
    changePage(true);
}

void
WizardDialog::on_newAccountButton_clicked()
{
    changePage(false);
}

void
WizardDialog::changePage(bool existingAccount)
{
    ui->stackedWidget->setCurrentIndex(1);

    ui->avatarButton->setHidden(existingAccount);
    ui->ringLogo->setHidden(existingAccount);
    ui->label->setHidden(existingAccount);
    ui->usernameEdit->setHidden(existingAccount);
    ui->confirmPasswordEdit->setHidden(existingAccount);

    ui->pinEdit->setVisible(existingAccount);
}

void
WizardDialog::on_previousButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
