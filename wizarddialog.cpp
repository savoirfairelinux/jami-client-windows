 /**************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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
#include <QFileDialog>

#include "accountmodel.h"
#include "account.h"
#include "profilemodel.h"
#include "profile.h"
#include "namedirectory.h"

#include "utils.h"

const QString DEFAULT_RING_ACCT_ALIAS = QObject::tr("Ring account", "Default alias for new Ring account");

WizardDialog::WizardDialog(WizardMode wizardMode, Account* toBeMigrated, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::WizardDialog),
    account_(toBeMigrated),
    wizardMode_(wizardMode),
    nameLookupTimer_(this)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);

    QPixmap logo(":/images/logo-ring-standard-coul.png");

    ui->welcomeLogo->setPixmap(logo.scaledToHeight(65, Qt::SmoothTransformation));
    ui->welcomeLogo->setAlignment(Qt::AlignHCenter);

    ui->fullNameEdit->setText(Utils::GetCurrentUserName());

    movie_ = new QMovie(":/images/loading.gif");
    ui->spinnerLabel->setMovie(movie_);
    movie_->start();

    if (wizardMode_ == MIGRATION) {
        Utils::slidePage(ui->stackedWidget, ui->profilePage);
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->setText(toBeMigrated->displayName());
        ui->previousButton->hide();
        ui->photoBooth->hide();
        ui->pinEdit->hide();
        ui->infoLabel->setText(tr("Your account needs to be migrated. Enter your password."));
    } else {
        ui->navBarWidget->hide();
    }

    ui->searchingStateLabel->clear();
    connect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
            this, SLOT(handle_registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)));

    nameLookupTimer_.setSingleShot(true);
    connect(&nameLookupTimer_, QTimer::timeout, this, WizardDialog::timeoutNameLookupTimer);
    connect(ui->photoBooth, &PhotoboothWidget::photoTaken, this, WizardDialog::on_photoTaken);
    ui->avatarLabel->hide();
}

WizardDialog::~WizardDialog()
{
    disconnect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
               this, SLOT(handle_registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)));
    delete ui;
}

void
WizardDialog::processWizardInformations()
{
    if (wizardMode_ == MIGRATION)
        ui->progressLabel->setText(tr("Migrating your Ring account..."));
    else if (wizardMode_ == IMPORT)
        ui->progressLabel->setText(tr("Importing account archive..."));
    else
        ui->progressLabel->setText(tr("Generating your Ring account..."));

    if (wizardMode_ != IMPORT) {
        if (ui->pinEdit->isVisible() && ui->pinEdit->text().isEmpty()) {
            ui->pinEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
            return;
        }

        if (ui->passwordEdit->text() != ui->confirmPasswordEdit->text()) {
            ui->passwordEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
            ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(204, 0, 0);");
            return;
        } else {
            ui->passwordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
            ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
            ui->pinEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        }

        QString accountAlias = (ui->fullNameEdit->text().isEmpty() ||
                                ui->fullNameEdit->text().isNull()) ? DEFAULT_RING_ACCT_ALIAS : ui->fullNameEdit->text();
        QString archivePin = (ui->pinEdit->text().isEmpty() || ui->pinEdit->text().isNull()) ? QString() : ui->pinEdit->text();

        createRingAccount(accountAlias, ui->passwordEdit->text(), archivePin);

        ui->passwordEdit->clear();
        ui->confirmPasswordEdit->clear();
        ui->pinEdit->clear();
    }

    ui->navBarWidget->hide();
    Utils::slidePage(ui->stackedWidget, ui->spinnerPage);
    repaint();
    Utils::CreateStartupLink();
}

void
WizardDialog::endSetup(Account::RegistrationState state)
{
#pragma push_macro("ERROR")
#undef ERROR
    switch (state) {
        case Account::RegistrationState::UNREGISTERED:
        case Account::RegistrationState::READY:
            if (ui->signUpCheckbox->isChecked()) { // If the user wants to register its name on the blockchain
                bool regSuccess = account_->registerName(ui->passwordEdit->text(), ui->usernameEdit->text());
                ui->usernameEdit->clear();
                if (!regSuccess) {
                    usernameFailedRegistration();
                } else {
                    connect(account_, SIGNAL(nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)),
                            this, SLOT(handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus,QString)));
                    ui->progressLabel->setText(tr("Registering your public username, it may take a few minutes..."));
                }
            } else {
                account_->performAction(Account::EditAction::RELOAD);
                accept();
            }

            { //avoid scope crossing init
                auto profile = ProfileModel::instance().selectedProfile();
                if (profile && AccountModel::instance().size() == 1) {
                    profile->setAccounts({account_});
                    profile->save();
                }
            }
            break;
        case Account::RegistrationState::ERROR:
            ui->spinnerLabel->hide();
            ui->navBarWidget->show();
            ui->nextButton->hide();
            ui->progressLabel->setText(tr("An error has occured during your account creation"));
            AccountModel::instance().remove(account_);
            break;
        case Account::RegistrationState::TRYING:
        case Account::RegistrationState::COUNT__:
            break;
    }
#pragma pop_macro("ERROR")
}

void
WizardDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    if (wizardMode_ == WIZARD){
        done(QDialog::Rejected);
    } else {
        QDialog::closeEvent(event);
    }
}

void
WizardDialog::usernameFailedRegistration()
{
    QMessageBox::warning(this, "Username not registered", "Your account has been created, but we could not register your username. Try again from the settings menu.");
}

void
WizardDialog::on_photoTaken(QString fileName)
{
    auto image = QImage(fileName);
    auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
    ui->photoBooth->stopBooth();
    ui->avatarLabel->show();
}

void
WizardDialog::on_existingPushButton_clicked()
{
    ui->navBarWidget->show();
    ui->nextButton->hide();
    Utils::slidePage(ui->stackedWidget, ui->linkMethodPage);
}

void
WizardDialog::on_newAccountButton_clicked()
{
    wizardMode_ = NEW_ACCOUNT;
    changePage(false);
}

void
WizardDialog::changePage(bool existingAccount)
{
    if (existingAccount) { // If user want to add a device
        ui->accountLabel->setText(tr("Add a device"));
        Utils::slidePage(ui->stackedWidget, ui->explanationPage);
        ui->photoBooth->hide();
    } else { // If user want to create a new account
        ui->accountLabel->setText(tr("Create your account"));
        Utils::slidePage(ui->stackedWidget, ui->profilePage);
        ui->photoBooth->startBooth();
        ui->photoBooth->show();
    }
    ui->navBarWidget->show();
    ui->avatarLabel->setHidden(true);
    ui->infoLabel->setHidden(existingAccount);
    ui->usernameEdit->setHidden(existingAccount);
    ui->signUpCheckbox->setHidden(existingAccount);
    ui->signUpCheckbox->setChecked(!existingAccount);
    ui->confirmPasswordEdit->setHidden(existingAccount);
    ui->searchingStateLabel->setHidden(existingAccount);

    ui->pinEdit->setVisible(existingAccount);
}

void
WizardDialog::on_nextButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();

    if (curWidget == ui->profilePage || curWidget == ui->explanationPage) {
        Utils::slidePage(ui->stackedWidget, ui->accountPage);
    } else if (curWidget == ui->accountPage) {
        processWizardInformations();
    } else if (curWidget == ui->fileImportPage) {
        validateFileImport();
    }
}

void
WizardDialog::on_previousButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();

    if (curWidget == ui->profilePage) {
        ui->navBarWidget->hide();
        Utils::slidePage(ui->stackedWidget, ui->welcomePage);
    } else if (curWidget == ui->explanationPage || curWidget == ui->fileImportPage) {
        ui->navBarWidget->show();
        ui->nextButton->hide();
        Utils::slidePage(ui->stackedWidget, ui->linkMethodPage);
    } else if (curWidget == ui->accountPage) {

        if (ui->pinEdit->isVisible()) // If we are adding a device
            Utils::slidePage(ui->stackedWidget, ui->explanationPage);
        else // If we are creating a new account
            Utils::slidePage(ui->stackedWidget, ui->profilePage);

        ui->passwordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->pinEdit->setStyleSheet("border-color: rgb(0, 192, 212);");

    } else if (curWidget == ui->linkMethodPage) {
        ui->navBarWidget->hide();
        ui->nextButton->show();
        Utils::slidePage(ui->stackedWidget, ui->welcomePage);
    } else if (curWidget == ui->spinnerPage) {
        ui->nextButton->show();
        ui->navBarWidget->hide();
        Utils::slidePage(ui->stackedWidget, ui->welcomePage);
    }
}

void
WizardDialog::on_passwordEdit_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)

    if (ui->pinEdit->isVisible()) {
        ui->confirmPasswordEdit->setText(ui->passwordEdit->text());
    }
}

void
WizardDialog::on_usernameEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    if(ui->signUpCheckbox->isChecked() && !arg1.isEmpty()) {
        ui->searchingStateLabel->setText(tr("Searching..."));
        nameLookupTimer_.start(1500);
    } else {
        ui->searchingStateLabel->clear();
        nameLookupTimer_.stop();
    }
}

void
WizardDialog::timeoutNameLookupTimer()
{
    if(ui->signUpCheckbox->isChecked() && !ui->usernameEdit->text().isEmpty()) {
        NameDirectory::instance().lookupName(nullptr, QString(), ui->usernameEdit->text());
    } else {
        ui->searchingStateLabel->clear();
    }
}

void
WizardDialog::handle_registeredNameFound (Account* account, NameDirectory::LookupStatus status, const QString& address, const QString& name)
{
    Q_UNUSED(account)
    Q_UNUSED(address)
    Q_UNUSED(name)
#pragma push_macro("ERROR")
#undef ERROR
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
    accept();
}

void
WizardDialog::on_signUpCheckbox_toggled(bool checked)
{
    if (checked) {
        ui->usernameEdit->setEnabled(true);
    } else {
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->clear();
    }
}

void
WizardDialog::validateFileImport()
{
    // reset original color
    ui->archivePasswordInput->setStyleSheet("border-color: rgb(0, 192, 212);");

    // A file has been selected
    if (! ui->archivePathSelector->text().isEmpty() && ! ui->archivePathSelector->text().isNull()){
        ui->archivePasswordInput->setEnabled(true);
        Profile *profile = ProfileModel::instance().selectedProfile();
        ui->signUpCheckbox->setChecked(false);
        createRingAccount(profile->person()->formattedName(),
                          ui->archivePasswordInput->text(),
                          QString(),
                          ui->archivePathSelector->text());
        processWizardInformations();
    }
}

void WizardDialog::on_archivePathSelector_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this,
                                            tr("Open File"),
                                            QString(),
                                            tr("Ring archive files (*.gz);; All files (*)"));

    // for export get current account archive path
    // for import use set path
    // check if archive has password
    // if it has, prompt for password
    filePath = QDir::toNativeSeparators(filePath);
    ui->archivePathSelector->setText(filePath);
}

void
WizardDialog::createRingAccount(const QString &displayName,
                                const QString &password,
                                const QString &pin,
                                const QString &archivePath)
{
    QString alias = (displayName.isEmpty() || displayName.isNull()) ? DEFAULT_RING_ACCT_ALIAS :
                                                                      displayName;
    // set display name
    account_ = AccountModel::instance().add(alias, Account::Protocol::RING);
    account_->setDisplayName(alias);

    // archive properties
    account_->setArchivePassword(password);
    // import from DHT
    if (!pin.isEmpty() && !pin.isNull())
        account_->setArchivePin(pin);
    // import from file
    if (!archivePath.isEmpty() && !archivePath.isNull())
        account_->setArchivePath(archivePath);

    // set default UPNP behavior
    account_->setUpnpEnabled(true);

    //set default ringtone
    account_->setRingtonePath(Utils::GetRingtonePath());

    connect(account_, &Account::stateChanged, this, &WizardDialog::endSetup);

    account_->performAction(Account::EditAction::SAVE);
    auto profile = ProfileModel::instance().selectedProfile();
    if (profile && AccountModel::instance().size() == 1) {
        profile->person()->setFormattedName(alias);
    }

}

void
WizardDialog::on_dhtImportBtn_clicked()
{
    ui->nextButton->show();
    changePage(true);
}

void
WizardDialog::on_fileImportBtn_clicked()
{
    ui->navBarWidget->show();
    ui->nextButton->show();
    wizardMode_ = IMPORT;
    Utils::slidePage(ui->stackedWidget, ui->fileImportPage);

}
