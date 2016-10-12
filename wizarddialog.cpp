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
#include <QMessageBox>

#include "accountmodel.h"
#include "account.h"
#include "profilemodel.h"
#include "profile.h"

#include "utils.h"
#include "photoboothdialog.h"

WizardDialog::WizardDialog(WizardMode wizardMode, Account* toBeMigrated, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog),
    account_(toBeMigrated),
    wizardMode_(wizardMode)
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

    movie_ = new QMovie(":/images/loading.gif");
    ui->spinnerLabel->setMovie(movie_);
    movie_->start();

    if (wizardMode_ == MIGRATION) {
        ui->stackedWidget->setCurrentIndex(1);
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->setText(toBeMigrated->displayName());
        ui->previousButton->hide();
        ui->avatarButton->hide();
        ui->pinEdit->hide();
        ui->label->setText(tr("Your account needs to be migrated. Choose a password."));
    }
    ui->searchingStateLabel->clear();
    connect(&NameDirectory::instance(), SIGNAL(registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
            this, SLOT(handle_registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)));
}

WizardDialog::~WizardDialog()
{
    disconnect(&NameDirectory::instance(), SIGNAL(registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
               this, SLOT(handle_registeredNameFound(const Account*,NameDirectory::LookupStatus,const QString&,const QString&)));
    delete ui;
}

void
WizardDialog::accept()
{
    if (ui->pinEdit->isVisible() && ui->pinEdit->text().isEmpty()) {
        ui->pinEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
        return;
    }
    if (ui->passwordEdit->text().isEmpty() || ui->passwordEdit->text() != ui->confirmPasswordEdit->text()) {
        ui->passwordEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
        ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
        return;
    } else {
        ui->passwordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->pinEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
    }

    ui->stackedWidget->setCurrentIndex(2);

    auto profile = ProfileModel::instance().selectedProfile();

    repaint();

    Utils::CreateStartupLink();

    if (account_ == nullptr) {
        account_ = AccountModel::instance().add(ui->usernameEdit->text(), Account::Protocol::RING);
        if (not ui->usernameEdit->text().isEmpty()) {
            account_->setDisplayName(ui->usernameEdit->text());
            profile->person()->setFormattedName(ui->usernameEdit->text());
        }
        else {
            profile->person()->setFormattedName(tr("Unknown"));
        }
    }
    account_->setRingtonePath(Utils::GetRingtonePath());
    account_->setUpnpEnabled(true);

    account_->setArchivePassword(ui->passwordEdit->text());
    ui->passwordEdit->setEnabled(false);
    ui->confirmPasswordEdit->setEnabled(false);

    if (not ui->pinEdit->text().isEmpty()) {
        account_->setArchivePin(ui->pinEdit->text());
    }

    connect(account_, SIGNAL(stateChanged(Account::RegistrationState)),
            this, SLOT(endSetup(Account::RegistrationState)));

    account_->performAction(Account::EditAction::SAVE);

    profile->setAccounts({account_});
    profile->save();
}

void
WizardDialog::endSetup(Account::RegistrationState state)
{
    #pragma push_macro("ERROR")
    #undef ERROR

    switch (state) {
    case Account::RegistrationState::READY:
    {
        if (ui->signUpCheckbox->isChecked()) { // If the user wants to register its name on the blockchain
            bool regSuccess = account_->registerName(ui->passwordEdit->text(), ui->usernameEdit->text());
            if (!regSuccess) usernameFailedRegistration();
            else
                connect(account_, SIGNAL(nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)),
                        this, SLOT(handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)));
        } else {
            account_->performAction(Account::EditAction::RELOAD);
            QDialog::accept();
        }
        break;
    }
    case Account::RegistrationState::UNREGISTERED:
    case Account::RegistrationState::TRYING:
    case Account::RegistrationState::COUNT__:
        break;
    case Account::RegistrationState::ERROR:
    {
        ui->stackedWidget->setCurrentIndex(3);

        ui->passwordEdit->clear();
        ui->confirmPasswordEdit->clear();
        ui->pinEdit->clear();
        ui->passwordEdit->setEnabled(true);
        ui->confirmPasswordEdit->setEnabled(true);
    }
    }
    #pragma pop_macro("ERROR")
}

void
WizardDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    if (wizardMode_ == WIZARD)
        exit(0);
    else
        QDialog::closeEvent(event);
}

void
WizardDialog::usernameFailedRegistration()
{
    QMessageBox::warning(this, "Username not registered", "Your account has been created, but we could not register your username. Try again from the settings menu.");
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
    ui->passwordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
    ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
    ui->pinEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
}

void
WizardDialog::on_passwordEdit_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)

    if (ui->pinEdit->isVisible()) {
        ui->confirmPasswordEdit->setText(ui->passwordEdit->text());
    }
}

void WizardDialog::on_errorPushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void
WizardDialog::on_usernameEdit_textChanged(const QString &arg1)
{
    if(ui->signUpCheckbox->isChecked() && !arg1.isEmpty()) {
        ui->searchingStateLabel->setText(tr("Searching..."));
        NameDirectory::instance().lookupName(nullptr, QString(), arg1);
    }
}

void
WizardDialog::handle_registeredNameFound (const Account* account, NameDirectory::LookupStatus status, const QString& address, const QString& name)
{
    Q_UNUSED(account)
    Q_UNUSED(address)
#pragma push_macro("ERROR")
#undef ERROR
    if(name.compare(ui->usernameEdit->text())) { // If we received a lookup update about the name currently displayed
        switch(status) {
        case NameDirectory::LookupStatus::SUCCESS:
            ui->searchingStateLabel->setText(tr("Username not available."));
            break;
        case NameDirectory::LookupStatus::NOT_FOUND:
            ui->searchingStateLabel->setText(tr("Username is available."));
            break;
        case NameDirectory::LookupStatus::INVALID_NAME:
            ui->searchingStateLabel->setText(tr("Username is invalid."));
            break;
        case NameDirectory::LookupStatus::ERROR:
            ui->searchingStateLabel->setText(tr("Network error."));
            break;
        }
    }
#pragma pop_macro("ERROR")
}

void
WizardDialog::handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name)
{
    Q_UNUSED(name)

    disconnect(account_, SIGNAL(nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)),
               this, SLOT(handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)));
    if(status != NameDirectory::RegisterNameStatus::SUCCESS) {
        usernameFailedRegistration();
    }
    account_->performAction(Account::EditAction::RELOAD);
    QDialog::accept();
}
