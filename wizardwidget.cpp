/**************************************************************************
* Copyright (C) 2015-2018 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
* Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "wizardwidget.h"
#include "ui_wizardwidget.h"

#include <QMovie>
#include <QMessageBox>
#include <QFileDialog>

#include "accountmodel.h"
#include "account.h"
#include "profilemodel.h"
#include "profile.h"
#include "namedirectory.h"

#include "utils.h"

const QString DEFAULT_RING_ACCT_ALIAS = QObject::tr("Jami account", "Default alias for new Jami account");

WizardWidget::WizardWidget(WizardMode wizardMode, AccountInfo* toBeMigrated, QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::WizardWidget),
    account_(toBeMigrated),
    wizardMode_(wizardMode),
    nameLookupTimer_(this)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo-jami-standard-coul.png");

    ui->welcomeLogo->setPixmap(logo.scaledToHeight(65, Qt::SmoothTransformation));
    ui->welcomeLogo->setAlignment(Qt::AlignHCenter);

    ui->fullNameEdit->setText(Utils::GetCurrentUserName());

    movie_ = new QMovie(":/images/loading.gif");
    ui->spinnerLabel->setMovie(movie_);
    movie_->start();

    if (wizardMode_ == MIGRATION) {
        Utils::setStackWidget(ui->stackedWidget, ui->profilePage);
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->setText(QString::fromStdString(toBeMigrated->profileInfo.alias));
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
    connect(&nameLookupTimer_, &QTimer::timeout, this, &WizardWidget::timeoutNameLookupTimer);
    connect(ui->photoBooth, &PhotoboothWidget::photoTaken, this, &WizardWidget::on_photoTaken);
    ui->avatarLabel->hide();

    ui->containerWidget->setVisible(false);
}

WizardWidget::~WizardWidget()
{
    disconnect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)),
               this, SLOT(handle_registeredNameFound(Account*,NameDirectory::LookupStatus,const QString&,const QString&)));
    delete ui;
}

void
WizardWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
    Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
}

void
WizardWidget::processWizardInformations()
{
    if (wizardMode_ == MIGRATION)
        ui->progressLabel->setText(tr("Migrating your Jami account..."));
    else if (wizardMode_ == IMPORT)
        ui->progressLabel->setText(tr("Importing account archive..."));
    else
        ui->progressLabel->setText(tr("Generating your Jami account..."));

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

    Utils::CreateStartupLink();
}

void
WizardWidget::on_photoTaken(QString fileName)
{
    auto image = QImage(fileName);
    auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
    ui->photoBooth->stopBooth();
    ui->avatarLabel->show();
}

void
WizardWidget::on_existingPushButton_clicked()
{
    ui->navBarWidget->show();
    ui->nextButton->hide();
    Utils::setStackWidget(ui->stackedWidget, ui->linkMethodPage);
}

void
WizardWidget::on_newAccountButton_clicked()
{
    wizardMode_ = NEW_ACCOUNT;
    changePage(false);
}

void
WizardWidget::changePage(bool existingAccount)
{
    if (existingAccount) { // If user want to add a device
        ui->accountLabel->setText(tr("Add a device"));
        Utils::setStackWidget(ui->stackedWidget, ui->explanationPage);
        ui->photoBooth->hide();
    } else { // If user want to create a new account
        ui->accountLabel->setText(tr("Create your account"));
        Utils::setStackWidget(ui->stackedWidget, ui->profilePage);
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
WizardWidget::on_nextButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    if (curWidget == ui->profilePage) {
        ui->photoBooth->stopBooth();
    }
    if (curWidget == ui->profilePage || curWidget == ui->explanationPage) {
        Utils::setStackWidget(ui->stackedWidget, ui->accountPage);
    } else if (curWidget == ui->accountPage) {
        processWizardInformations();
    } else if (curWidget == ui->fileImportPage) {
        validateFileImport();
    }
}

void
WizardWidget::on_previousButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();

    if (curWidget == ui->profilePage) {
        ui->navBarWidget->hide();
        Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
    } else if (curWidget == ui->explanationPage || curWidget == ui->fileImportPage) {
        ui->navBarWidget->show();
        ui->nextButton->hide();
        Utils::setStackWidget(ui->stackedWidget, ui->linkMethodPage);
    } else if (curWidget == ui->accountPage) {

        if (ui->pinEdit->isVisible()) {
            // If we are adding a device
            Utils::setStackWidget(ui->stackedWidget, ui->explanationPage);
        } else {
            ui->photoBooth->startBooth();
            ui->photoBooth->show();
            Utils::setStackWidget(ui->stackedWidget, ui->profilePage);
        }

        ui->passwordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->confirmPasswordEdit->setStyleSheet("border-color: rgb(0, 192, 212);");
        ui->pinEdit->setStyleSheet("border-color: rgb(0, 192, 212);");

    } else if (curWidget == ui->linkMethodPage) {
        ui->navBarWidget->hide();
        ui->nextButton->show();
        Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
    } else if (curWidget == ui->spinnerPage) {
        ui->nextButton->show();
        ui->navBarWidget->hide();
        Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
    }
}

void
WizardWidget::on_passwordEdit_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)

    if (ui->pinEdit->isVisible()) {
        ui->confirmPasswordEdit->setText(ui->passwordEdit->text());
    }
}

void
WizardWidget::on_usernameEdit_textChanged(const QString &arg1)
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
WizardWidget::timeoutNameLookupTimer()
{
    if(ui->signUpCheckbox->isChecked() && !ui->usernameEdit->text().isEmpty()) {
        NameDirectory::instance().lookupName(nullptr, QString(), ui->usernameEdit->text());
    } else {
        ui->searchingStateLabel->clear();
    }
}

void
WizardWidget::handle_registeredNameFound (Account* account, NameDirectory::LookupStatus status, const QString& address, const QString& name)
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
WizardWidget::handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name)
{
    Q_UNUSED(name);
    Q_UNUSED(status);
}

void
WizardWidget::on_signUpCheckbox_toggled(bool checked)
{
    if (checked) {
        ui->usernameEdit->setEnabled(true);
    } else {
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->clear();
    }
}

void
WizardWidget::validateFileImport()
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

void WizardWidget::on_archivePathSelector_clicked()
{
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this,
                                            tr("Open File"),
                                            QString(),
                                            tr("Jami archive files (*.gz);; All files (*)"));

    // for export get current account archive path
    // for import use set path
    // check if archive has password
    // if it has, prompt for password
    filePath = QDir::toNativeSeparators(filePath);
    ui->archivePathSelector->setText(filePath);
}

void
WizardWidget::createRingAccount(const QString &displayName,
                                const QString &password,
                                const QString &pin,
                                const QString &archivePath)
{
    QtConcurrent::run(
        [=] {
            LRCInstance::accountModel().createNewAccount(
                lrc::api::profile::Type::RING,
                displayName.toStdString(),
                archivePath.toStdString(),
                password.toStdString(),
                pin.toStdString()
            );
        });

    QMetaObject::Connection* toDisconnect = &accountAddedConnection_;
    accountAddedConnection_ = connect(&LRCInstance::accountModel(),
        &lrc::api::NewAccountModel::accountAdded,
        [this, toDisconnect](const std::string& accountId) {
            //set default ringtone
            auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
            confProps.Ringtone.ringtonePath = Utils::GetRingtonePath().toStdString();
            LRCInstance::accountModel().setAccountConfig(accountId, confProps);
            emit NavigationRequested(ScreenEnum::CallScreen);
            QObject::disconnect(*toDisconnect);
        });


    ui->navBarWidget->hide();
    Utils::setStackWidget(ui->stackedWidget, ui->spinnerPage);
    repaint();
}

void
WizardWidget::on_dhtImportBtn_clicked()
{
    ui->nextButton->show();
    changePage(true);
}

void
WizardWidget::on_fileImportBtn_clicked()
{
    ui->navBarWidget->show();
    ui->nextButton->show();
    wizardMode_ = IMPORT;
    Utils::setStackWidget(ui->stackedWidget, ui->fileImportPage);

}
