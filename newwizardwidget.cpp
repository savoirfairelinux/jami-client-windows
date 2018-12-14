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

#include "newwizardwidget.h"
#include "ui_newwizardwidget.h"

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

NewWizardWidget::NewWizardWidget(WizardMode wizardMode, AccountInfo* toBeMigrated, QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::NewWizardWidget),
    account_(toBeMigrated),
    wizardMode_(wizardMode),
    nameLookupTimer_(this)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo-jami-standard-coul.png");

    ui->welcomeLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->welcomeLogo->setAlignment(Qt::AlignHCenter);

    ui->fullNameEdit->setText(Utils::GetCurrentUserName());

    movie_ = new QMovie(":/images/loading.gif");
    ui->spinnerLabel->setMovie(movie_);
    movie_->start();

    if (wizardMode_ == MIGRATION) {
        changePage(ui->createRingAccountPage);
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->setText(QString::fromStdString(toBeMigrated->profileInfo.alias));
        ui->previousButton->hide();
        ui->photoBooth->hide();
        ui->infoWidget->show();
        ui->infoLabel->setText(tr("Your account needs to be migrated. Enter your password."));
    } else {
        ui->infoWidget->hide();
        ui->navBarWidget->hide();
    }

    //ui->searchingStateLabel->clear();

    connect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*, NameDirectory::LookupStatus, const QString&, const QString&)),
            this, SLOT(handle_registeredNameFound(Account*, NameDirectory::LookupStatus, const QString&, const QString&)));

    nameLookupTimer_.setSingleShot(true);

    connect(&nameLookupTimer_, &QTimer::timeout,
            this, &NewWizardWidget::timeoutNameLookupTimer);

    connect(ui->photoBooth, &PhotoboothWidget::photoTaken,
            this, &NewWizardWidget::on_photoTaken);

    connect(ui->backButton, &QPushButton::clicked,
        [this] {
            emit NavigationRequested(ScreenEnum::CallScreen);
        });

    //ui->accountSectionInnerWidget->setStyleSheet("border-color:black; border: 2px;");

    ui->containerWidget->setVisible(false);
}

NewWizardWidget::~NewWizardWidget()
{
    disconnect(&NameDirectory::instance(), SIGNAL(registeredNameFound(Account*, NameDirectory::LookupStatus, const QString&, const QString&)),
               this, SLOT(handle_registeredNameFound(Account*, NameDirectory::LookupStatus, const QString&, const QString&)));
    delete ui;
}

void
NewWizardWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
    changePage(ui->welcomePage);
    Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
}

void
NewWizardWidget::processWizardInformations()
{
    if (wizardMode_ == MIGRATION)
        ui->progressLabel->setText(tr("Migrating your Jami account..."));
    else if (wizardMode_ == IMPORT)
        ui->progressLabel->setText(tr("Importing account archive..."));
    else
        ui->progressLabel->setText(tr("Generating your Jami account..."));

    Utils::CreateStartupLink();
}

void
NewWizardWidget::on_photoTaken(QString fileName)
{
    auto image = QImage(fileName);
    auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
    //ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
    ui->photoBooth->stopBooth();
    //ui->avatarLabel->show();
}

void
NewWizardWidget::on_existingPushButton_clicked()
{
    changePage(ui->linkRingAccountPage);
}

void
NewWizardWidget::on_newAccountButton_clicked()
{
    wizardMode_ = NEW_ACCOUNT;
    changePage(ui->createRingAccountPage);
}

void NewWizardWidget::changePage(QWidget* toPage)
{
    if (toPage == ui->welcomePage) {
        ui->navBarWidget->hide();
        if (LRCInstance::accountModel().getAccountList().size()) {
            ui->backButtonLayoutWidget->show();
        }
    } else if (toPage == ui->createRingAccountPage) {
        ui->navBarWidget->show();
        ui->backButtonLayoutWidget->hide();
    } else if (toPage == ui->linkRingAccountPage) {
        ui->navBarWidget->show();
        ui->backButtonLayoutWidget->hide();
    } else if (toPage == ui->spinnerPage) {
        ui->navBarWidget->hide();
        ui->backButtonLayoutWidget->hide();
    }

    Utils::setStackWidget(ui->stackedWidget, toPage);
}

void
NewWizardWidget::on_nextButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    if (curWidget != ui->createRingAccountPage) {
        ui->photoBooth->stopBooth();
    }
    if (curWidget == ui->createRingAccountPage) {
        processWizardInformations();
        // or
        //validateFileImport();
    }
}

void
NewWizardWidget::on_previousButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    if (curWidget != ui->createRingAccountPage) {
        ui->photoBooth->stopBooth();
    }

    if (curWidget == ui->createRingAccountPage ||
        curWidget == ui->linkRingAccountPage) {
        changePage(ui->welcomePage);
    }
}

void
NewWizardWidget::on_passwordEdit_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
    validateProgression();
}

void
NewWizardWidget::on_usernameEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if (ui->signUpCheckbox->isChecked() && !arg1.isEmpty()) {
        //ui->searchingStateLabel->setText(tr("Searching..."));
        nameLookupTimer_.start(1500);
    } else {
       // ui->searchingStateLabel->clear();
        nameLookupTimer_.stop();
    }
    validateProgression();
}

void
NewWizardWidget::timeoutNameLookupTimer()
{
    if (ui->signUpCheckbox->isChecked() && !ui->usernameEdit->text().isEmpty()) {
        NameDirectory::instance().lookupName(nullptr, QString(), ui->usernameEdit->text());
    } else {
        //ui->searchingStateLabel->clear();
    }
}

void
NewWizardWidget::handle_registeredNameFound(Account* account, NameDirectory::LookupStatus status, const QString& address, const QString& name)
{
    Q_UNUSED(account);
    Q_UNUSED(address);
    Q_UNUSED(name);
#pragma push_macro("ERROR")
#undef ERROR
    switch (status) {
    case NameDirectory::LookupStatus::SUCCESS:
        //ui->searchingStateLabel->setText(tr("Username not available."));
        break;
    case NameDirectory::LookupStatus::NOT_FOUND:
        //ui->searchingStateLabel->setText(tr("Username is available."));
        break;
    case NameDirectory::LookupStatus::INVALID_NAME:
        //ui->searchingStateLabel->setText(tr("Username is invalid."));
        break;
    case NameDirectory::LookupStatus::ERROR:
        //ui->searchingStateLabel->setText(tr("Network error."));
        break;
    }
#pragma pop_macro("ERROR")
    validateProgression();
}

void
NewWizardWidget::handle_nameRegistrationEnded(NameDirectory::RegisterNameStatus status, const QString& name)
{
    Q_UNUSED(name);
    Q_UNUSED(status);
}

void
NewWizardWidget::on_signUpCheckbox_toggled(bool checked)
{
    if (checked) {
        ui->usernameEdit->setEnabled(true);
    } else {
        ui->usernameEdit->setEnabled(false);
        ui->usernameEdit->clear();
    }
    validateProgression();
}

void
NewWizardWidget::validateFileImport()
{
    // reset original color
    //ui->archivePasswordInput->setStyleSheet("border-color: rgb(0, 192, 212);");

    //// A file has been selected
    //if (!ui->archivePathSelector->text().isEmpty() && !ui->archivePathSelector->text().isNull()) {
    //    ui->archivePasswordInput->setEnabled(true);
    //    Profile *profile = ProfileModel::instance().selectedProfile();
    //    ui->signUpCheckbox->setChecked(false);
    //    createRingAccount(profile->person()->formattedName(),
    //        ui->archivePasswordInput->text(),
    //        QString(),
    //        ui->archivePathSelector->text());
    //    processWizardInformations();
    //}
    validateProgression();
}

void NewWizardWidget::validateProgression()
{
    qDebug() << "validate progression...";
}

void NewWizardWidget::on_archivePathSelector_clicked()
{
    //QString filePath;
    //filePath = QFileDialog::getOpenFileName(this,
    //    tr("Open File"),
    //    QString(),
    //    tr("Jami archive files (*.gz);; All files (*)"));

    //// for export get current account archive path
    //// for import use set path
    //// check if archive has password
    //// if it has, prompt for password
    //filePath = QDir::toNativeSeparators(filePath);
    //ui->archivePathSelector->setText(filePath);
}

void
NewWizardWidget::createRingAccount(const QString &displayName,
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
    changePage(ui->spinnerPage);
    repaint();
}