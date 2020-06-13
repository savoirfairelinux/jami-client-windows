/**************************************************************************
* Copyright (C) 2015-2018 by Savoir-faire Linux                           *
* Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
* Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
* Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
* Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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
#include <QBitmap>
#include <QtConcurrent/QtConcurrent>

#include "namedirectory.h"

#include "utils.h"
#include "ringthemeutils.h"

#undef REGISTERED
#include "../daemon/src/dring/account_const.h"

NewWizardWidget::NewWizardWidget(QWidget* parent) :
    NavWidget(parent),
    ui(new Ui::NewWizardWidget),
    wizardMode_(WizardMode::CREATE),
    lookupTimer_(this)
{
    ui->setupUi(this);

    QPixmap logo(":/images/logo-jami-standard-coul.png");

    ui->welcomeLogo->setPixmap(logo.scaledToHeight(100, Qt::SmoothTransformation));
    ui->welcomeLogo->setAlignment(Qt::AlignHCenter);

    creationSpinnerMovie_ = new QMovie(":/images/jami_eclipse_spinner.gif");
    ui->spinnerLabel->setMovie(creationSpinnerMovie_);
    creationSpinnerMovie_->start();

    lookupSpinnerMovie_ = new QMovie(":/images/jami_rolling_spinner.gif");
    lookupSpinnerMovie_->setScaledSize(QSize(30, 30));

    ui->lookupStatusLabel->setMovie(lookupSpinnerMovie_);
    ui->lookupStatusLabel->hide();

    registrationStateOk_ = false;

    statusSuccessPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-done-24px.svg", RingTheme::presenceGreen_);
    statusInvalidPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-error_outline-24px.svg", RingTheme::urgentOrange_);
    statusErrorPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-close-24px.svg", RingTheme::red_);

    setNavBarVisibility(false, true);

    lookupTimer_.setSingleShot(true);

    collapsiblePasswordWidget_ = new CollapsiblePasswordWidget(this);
    collapsiblePasswordWidget_->setParent(ui->expandablePasswordWidget);
    ui->expandablePasswordWidget->setLabelText("Choose a password for enchanced security");
    ui->expandablePasswordWidget->addExpandWidget(collapsiblePasswordWidget_);

    connect(ui->fileImportBtn, &QPushButton::clicked,
        [this] {
            QString filePath;
            filePath = QFileDialog::getOpenFileName(this,
                tr("Open File"),
                QString(),
                tr("Jami archive files") + " (*.gz);" + tr("All files") + " (*)");
            fileToImport_ = QDir::toNativeSeparators(filePath);
            if (!fileToImport_.isEmpty()) {
                QFileInfo fi(filePath);
                ui->fileImportBtn->setText(fi.fileName());
            } else {
                ui->fileImportBtn->setText(tr("Archive (None)"));
            }
            validateWizardProgression();
        });

    connect(&lookupTimer_, &QTimer::timeout,
            this, &NewWizardWidget::timeoutNameLookupTimer);

    connect(ui->backButton, &QPushButton::clicked,
        [this] {
            emit NavigationRequested(ScreenEnum::CallScreen);
        });

    connect(collapsiblePasswordWidget_->getPasswordEdit(), &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(collapsiblePasswordWidget_->getConfirmPasswordEdit(), &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(ui->pinFromDevice, &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(ui->usernameManagerEdit, &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(ui->accountManagerEdit, &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(ui->passwordManagerEdit, &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    ui->containerWidget->setVisible(false);
}

NewWizardWidget::~NewWizardWidget()
{
    delete ui;
}

void
NewWizardWidget::updateNameRegistrationUi(NameRegistrationUIState state)
{
    switch (state) {
    case NameRegistrationUIState::BLANK:
        ui->lookupStatusLabel->hide();
        break;
    case NameRegistrationUIState::INVALID:
        ui->lookupStatusLabel->setPixmap(statusInvalidPixmap_);
        break;
    case NameRegistrationUIState::TAKEN:
        ui->lookupStatusLabel->setPixmap(statusErrorPixmap_);
        break;
    case NameRegistrationUIState::FREE:
        ui->lookupStatusLabel->setPixmap(statusSuccessPixmap_);
        break;
    case NameRegistrationUIState::SEARCHING:
        ui->lookupStatusLabel->setMovie(lookupSpinnerMovie_);
        lookupSpinnerMovie_->stop();
        lookupSpinnerMovie_->start();
        ui->lookupStatusLabel->show();
        break;
    }
}

void
NewWizardWidget::navigated(bool to)
{
    ui->containerWidget->setVisible(to);
    changePage(ui->welcomePage);
    Utils::setStackWidget(ui->stackedWidget, ui->welcomePage);
}

void
NewWizardWidget::slotAccountListChanged()
{}

void
NewWizardWidget::on_fromDeviceButton_clicked()
{
    changePage(ui->importFromDevicePage);
}

void
NewWizardWidget::on_newAccountButton_clicked()
{
    changePage(ui->createAccountPage);
}

void
NewWizardWidget::on_showAdvancedButton_clicked()
{
    ui->newSIPAccountButton->setVisible(!ui->newSIPAccountButton->isVisible());
    ui->connectAccountManagerButton->setVisible(!ui->connectAccountManagerButton->isVisible());
}

void
NewWizardWidget::on_connectAccountManagerButton_clicked()
{
    changePage(ui->connectToAccountManagerPage);
}

void
NewWizardWidget::on_fromBackupButton_clicked()
{
    changePage(ui->importFromBackupPage);
}

void
NewWizardWidget::on_newSIPAccountButton_clicked()
{
    changePage(ui->createSIPAccountPage);
}

void NewWizardWidget::changePage(QWidget* toPage)
{
    if (toPage == ui->spinnerPage) {
        setNavBarVisibility(false);
    }
    Utils::setStackWidget(ui->stackedWidget, toPage);
    if (toPage == ui->welcomePage) {
        fileToImport_ = QString("");
        setNavBarVisibility(false, true);
        ui->lookupStatusLabel->hide();
        collapsiblePasswordWidget_->getPasswordStatusLabel()->hide();
        ui->newSIPAccountButton->hide();
        ui->connectAccountManagerButton->hide();
    } else if (toPage == ui->createAccountPage) {
        ui->usernameEdit->clear();
        collapsiblePasswordWidget_->getPasswordEdit()->clear();
        collapsiblePasswordWidget_->getConfirmPasswordEdit()->clear();
        ui->signUpCheckbox->setChecked(true);
        ui->usernameEdit->setEnabled(true);
        ui->fullNameEdit->setText(QString());
        setNavBarVisibility(true);
        updateCustomUI();
        registeredNameFoundConnection_ = connect(
            &LRCInstance::accountModel(), &lrc::api::NewAccountModel::registeredNameFound,
            this, &NewWizardWidget::slotRegisteredNameFound);
        validateWizardProgression();
        ui->setAvatarWidget->startBooth();
    } else if (toPage == ui->createSIPAccountPage) {
        ui->SIPusernameEdit->clear();
        ui->SIPpasswordEdit->clear();
        ui->SIPservernameEdit->clear();
        ui->SIPproxyEdit->clear();
        ui->SIPfullNameEdit->setText(QString());
        ui->SIPusernameEdit->setEnabled(true);
        setNavBarVisibility(true);
        ui->nextButton->setEnabled(true);
        ui->setSIPAvatarWidget->startBooth();
    } else if (toPage == ui->importFromDevicePage) {
        ui->pinFromDevice->clear();
        ui->passwordFromDevice->clear();
        setNavBarVisibility(true);
        ui->pinInfoLabel->hide();
    } else if (toPage == ui->spinnerPage) {
        ui->lookupStatusLabel->hide();
        collapsiblePasswordWidget_->getPasswordStatusLabel()->hide();
    } else if (toPage == ui->connectToAccountManagerPage) {
        setNavBarVisibility(true);
        ui->usernameManagerEdit->clear();
        ui->passwordManagerEdit->clear();
        ui->accountManagerEdit->clear();
        ui->nextButton->setEnabled(false);
    } else if (toPage == ui->importFromBackupPage) {
        setNavBarVisibility(true);
        ui->passwordFromBackupEdit->clear();
        fileToImport_ = QString("");
        ui->fileImportBtn->setText(tr("Archive (None)"));
        ui->nextButton->setEnabled(false);
        ui->backupInfoLabel->hide();
    } else if (toPage == ui->backupKeysPage) {
        setNavBarVisibility(false);
    }
}

void
NewWizardWidget::updateCustomUI()
{
}

void
NewWizardWidget::setNavBarVisibility(bool nav, bool back)
{
    ui->navBarWidget->setVisible(nav || back);
    ui->nextButton->setVisible(nav);
    ui->previousButton->setVisible(nav);
    ui->backButton->setVisible(back && LRCInstance::accountModel().getAccountList().size());
}

void
NewWizardWidget::on_nextButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    ui->setAvatarWidget->stopBooth();
    disconnect(registeredNameFoundConnection_);
    if (curWidget == ui->createAccountPage) {
        wizardMode_ = WizardMode::CREATE;
        processWizardInformations();
    } else if (curWidget == ui->importFromDevicePage) {
        wizardMode_ = WizardMode::IMPORT;
        processWizardInformations();
    } else if (curWidget == ui->createSIPAccountPage) {
        wizardMode_ = WizardMode::CREATESIP;
        processWizardInformations();
    } else if (curWidget == ui->connectToAccountManagerPage) {
        wizardMode_ = WizardMode::CONNECTMANAGER;
        processWizardInformations();
    } else if (curWidget == ui->importFromBackupPage) {
        wizardMode_ = WizardMode::IMPORT;
        processWizardInformations();
    }
}

void
NewWizardWidget::on_previousButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    if (curWidget == ui->createAccountPage) { ui->setAvatarWidget->stopBooth(); }
    if (curWidget == ui->createSIPAccountPage) { ui->setSIPAvatarWidget->stopBooth(); }
    disconnect(registeredNameFoundConnection_);
    ui->lookupStatusLabel->hide();
    collapsiblePasswordWidget_->getPasswordStatusLabel()->hide();
    if (curWidget == ui->createAccountPage ||
        curWidget == ui->importFromDevicePage ||
        curWidget == ui->createSIPAccountPage ||
        curWidget == ui->connectToAccountManagerPage ||
        curWidget == ui->importFromBackupPage) {
        changePage(ui->welcomePage);
    }
}

void
NewWizardWidget::on_pinInfoBtn_clicked()
{
    ui->pinInfoLabel->setVisible(!ui->pinInfoLabel->isVisible());
}

void
NewWizardWidget::on_backupInfoBtn_clicked()
{
    ui->backupInfoLabel->setVisible(!ui->backupInfoLabel->isVisible());
}

void
NewWizardWidget::on_neverShowAgainBox_clicked()
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::neverShowMeAgain, ui->neverShowAgainBox->isChecked());
}

void
NewWizardWidget::on_skipBtn_clicked()
{
    emit NavigationRequested(ScreenEnum::CallScreen);
    emit LRCInstance::instance().accountListChanged();
}

void
NewWizardWidget::on_exportBtn_clicked()
{
    if (Utils::exportAccount(this)) {
        emit NavigationRequested(ScreenEnum::CallScreen);
        emit LRCInstance::instance().accountListChanged();
    }
}

void
NewWizardWidget::on_usernameEdit_textChanged(const QString &arg1)
{
    registrationStateOk_ = false;
    if (ui->signUpCheckbox->isChecked() && !arg1.isEmpty()) {
        registeredName_ = ui->usernameEdit->text().simplified();
        lookupTimer_.start(200);
    } else {
        updateNameRegistrationUi(NameRegistrationUIState::BLANK);
        lookupTimer_.stop();
        if (!arg1.isEmpty()) {
            lookupTimer_.start(200);
        }
    }
    validateWizardProgression();
}

void
NewWizardWidget::timeoutNameLookupTimer()
{
    if (ui->signUpCheckbox->isChecked() && !ui->usernameEdit->text().isEmpty()) {
        updateNameRegistrationUi(NameRegistrationUIState::SEARCHING);
        NameDirectory::instance().lookupName(QString(), registeredName_);
    }
}

void
NewWizardWidget::slotRegisteredNameFound(const QString& accountId,
                                         LookupStatus status,
                                         const QString& address,
                                         const QString& name)
{
    Q_UNUSED(accountId);
    Q_UNUSED(address);

    using namespace lrc::api::account;
    if (name.length() < 3) {
        registrationStateOk_ = false;
        updateNameRegistrationUi(NameRegistrationUIState::INVALID);
    } else if (registeredName_ == name) {
        switch (status) {
        case LookupStatus::NOT_FOUND:
        case LookupStatus::ERROR:
            registrationStateOk_ = true;
            updateNameRegistrationUi(NameRegistrationUIState::FREE);
            break;
        case LookupStatus::INVALID_NAME:
        case LookupStatus::INVALID:
            registrationStateOk_ = false;
            updateNameRegistrationUi(NameRegistrationUIState::INVALID);
            break;
        case LookupStatus::SUCCESS:
            registrationStateOk_ = false;
            updateNameRegistrationUi(NameRegistrationUIState::TAKEN);
            break;
        }
    }
    validateWizardProgression();
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
    validateWizardProgression();
}

void
NewWizardWidget::validateWizardProgression()
{
    if (ui->stackedWidget->currentWidget() == ui->importFromDevicePage) {
        bool validPin = !ui->pinFromDevice->text().isEmpty();
        ui->nextButton->setEnabled(validPin);
        return;
    } else if (ui->stackedWidget->currentWidget() == ui->connectToAccountManagerPage) {
        bool validUsername = !ui->usernameManagerEdit->text().isEmpty();
        bool validPassword = !ui->passwordManagerEdit->text().isEmpty();
        bool validManager = !ui->accountManagerEdit->text().isEmpty();
        ui->nextButton->setEnabled(validUsername && validPassword && validManager);
        return;
    } else if (ui->stackedWidget->currentWidget() == ui->importFromBackupPage) {
        bool validImport = !fileToImport_.isEmpty();
        ui->nextButton->setEnabled(validImport);
        return;
    }
    bool usernameOk =
        !ui->signUpCheckbox->isChecked() ||
        (ui->signUpCheckbox->isChecked() &&
         !registeredName_.isEmpty() &&
         (registeredName_ == ui->usernameEdit->text()) &&
         registrationStateOk_ == true);
    bool passwordOk = collapsiblePasswordWidget_->getPasswordEdit()->text()
                      == collapsiblePasswordWidget_->getConfirmPasswordEdit()->text();
    if (passwordOk && !collapsiblePasswordWidget_->getPasswordEdit()->text().isEmpty()) {
        collapsiblePasswordWidget_->getPasswordStatusLabel()->show();
        collapsiblePasswordWidget_->getPasswordStatusLabel()->setPixmap(statusSuccessPixmap_);
    } else if (!passwordOk) {
        collapsiblePasswordWidget_->getPasswordStatusLabel()->show();
        collapsiblePasswordWidget_->getPasswordStatusLabel()->setPixmap(statusErrorPixmap_);
    } else {
        collapsiblePasswordWidget_->getPasswordStatusLabel()->hide();
    }
    ui->nextButton->setEnabled(usernameOk && passwordOk);
}

void
NewWizardWidget::processWizardInformations()
{
    inputPara_.clear();
    switch (wizardMode_) {
    case WizardMode::CREATE:
        ui->progressLabel->setText(tr("Generating your Jami account..."));
        inputPara_["alias"] = ui->fullNameEdit->text();
        inputPara_["password"] = collapsiblePasswordWidget_->getPasswordEdit()->text();
        ui->fullNameEdit->clear();
        collapsiblePasswordWidget_->getPasswordEdit()->clear();
        collapsiblePasswordWidget_->getConfirmPasswordEdit()->clear();
        break;
    case WizardMode::IMPORT:
        ui->progressLabel->setText(tr("Importing account archive..."));
        if (ui->stackedWidget->currentWidget() == ui->importFromBackupPage) {
            inputPara_["password"] = ui->passwordFromBackupEdit->text();
            ui->passwordFromBackupEdit->clear();
        } else if (ui->stackedWidget->currentWidget() == ui->importFromDevicePage) {
            inputPara_["archivePin"] = ui->pinFromDevice->text();
            inputPara_["password"] = ui->passwordFromDevice->text();
        }
        break;
    case WizardMode::MIGRATE:
        ui->progressLabel->setText(tr("Migrating your Jami account..."));
        break;
    case WizardMode::CREATESIP:
        ui->progressLabel->setText(tr("Generating your SIP account..."));
        if (ui->SIPfullNameEdit->text().isEmpty()) { inputPara_["alias"] = "SIP"; }
        else { inputPara_["alias"] = ui->SIPfullNameEdit->text(); }
        inputPara_["hostname"] = ui->SIPservernameEdit->text();
        inputPara_["username"] = ui->SIPusernameEdit->text();
        inputPara_["password"] = ui->SIPpasswordEdit->text();
        inputPara_["proxy"] = ui->SIPproxyEdit->text();
        break;
    case WizardMode::CONNECTMANAGER:
        ui->progressLabel->setText(tr("Connecting to account manager..."));
        inputPara_["username"] = ui->usernameManagerEdit->text();
        inputPara_["password"] = ui->passwordManagerEdit->text();
        inputPara_["manager"] = ui->accountManagerEdit->text();
        break;
    }

    inputPara_["archivePath"] = fileToImport_;
    if (inputPara_.find("archivePin") == inputPara_.end()) {
        inputPara_["archivePin"] = "";
    }

    changePage(ui->spinnerPage);
    createAccount();

    Utils::CreateStartupLink(L"Jami");
}

void
NewWizardWidget::createAccount()
{
    bool isConnectingToManager = wizardMode_ == WizardMode::CONNECTMANAGER;
    bool isRing = wizardMode_ == WizardMode::CREATE || wizardMode_ == WizardMode::IMPORT;
    bool isCreating = wizardMode_ == WizardMode::CREATE;
    if (isConnectingToManager) {
        Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountAdded,
            [this](const QString& accountId) {
                Q_UNUSED(accountId)
                if (!LRCInstance::accountModel().getAccountList().size())
                    return;
                emit NavigationRequested(ScreenEnum::CallScreen);
                emit LRCInstance::instance().accountListChanged();
        });
    } else {
        Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountAdded,
            [this, isRing, isCreating](const QString& accountId) {
                auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
                if (!isRing) {
                    // set SIP details
                    confProps.hostname = inputPara_["hostname"];
                    confProps.username = inputPara_["username"];
                    confProps.password = inputPara_["password"];
                    confProps.proxyServer = inputPara_["proxy"];
                }
                LRCInstance::accountModel().setAccountConfig(accountId, confProps);
                if (isRing) {
                    QSettings settings("jami.net", "Jami");
                    if (not settings.contains(SettingsKey::neverShowMeAgain)) {
                        settings.setValue(SettingsKey::neverShowMeAgain, false);
                    }
                    auto showBackup = isCreating && !settings.value(SettingsKey::neverShowMeAgain).toBool();

                    if (!confProps.username.isEmpty()) {
                        Utils::oneShotConnect(&LRCInstance::accountModel(),
                            &lrc::api::NewAccountModel::nameRegistrationEnded,
                            [this, showBackup] {
                                lrc::api::account::ConfProperties_t accountProperties = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
                                LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), accountProperties);
                                QSettings settings("jami.net", "Jami");
                                if (showBackup) {
                                    changePage(ui->backupKeysPage);
                                } else {
                                    emit NavigationRequested(ScreenEnum::CallScreen);
                                    emit LRCInstance::instance().accountListChanged();
                                }
                            });
                        LRCInstance::accountModel().registerName(
                            LRCInstance::getCurrAccId(),
                            inputPara_["password"],
                            registeredName_
                        );
                    } else {
                        if (showBackup) {
                            changePage(ui->backupKeysPage);
                        } else {
                            emit NavigationRequested(ScreenEnum::CallScreen);
                            emit LRCInstance::instance().accountListChanged();
                        }
                    }
                    LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
                } else {
                    LRCInstance::setCurrAccAvatar(ui->setSIPAvatarWidget->getAvatarPixmap());

                }
        });
    }
    Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountRemoved,
        [this](const QString& accountId) {
            Q_UNUSED(accountId);
            qWarning() << Q_FUNC_INFO << ": " << "accountRemoved";
            reportFailure();
        });
    Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::invalidAccountDetected,
        [this](const QString& accountId) {
            Q_UNUSED(accountId);
            qWarning() << Q_FUNC_INFO << ": " << "invalidAccountDetected";
            reportFailure();
        });
    QtConcurrent::run(
        [this, isRing, isConnectingToManager] {
            QMap<QString, QString> additionalAccountConfig;
            additionalAccountConfig.insert(DRing::Account::ConfProperties::Ringtone::PATH, Utils::GetRingtonePath());

            if (isConnectingToManager) {
                LRCInstance::accountModel().connectToAccountManager(
                    inputPara_["username"],
                    inputPara_["password"],
                    inputPara_["manager"],
                    additionalAccountConfig
                );
            } else if (isRing) {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::RING,
                    inputPara_["alias"],
                    inputPara_["archivePath"],
                    inputPara_["password"],
                    inputPara_["archivePin"],
                    "",
                    additionalAccountConfig
                );
            } else {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::SIP,
                    inputPara_["alias"],
                    inputPara_["archivePath"],
                    "",
                    "",
                    inputPara_["username"],
                    additionalAccountConfig
                );
                QThread::sleep(2);
                emit NavigationRequested(ScreenEnum::CallScreen);
                emit LRCInstance::instance().accountListChanged();
            }
    });
    changePage(ui->spinnerPage);
    repaint();
}

void
NewWizardWidget::reportFailure()
{
    auto spinnerGeometry = ui->spinnerLabel->frameGeometry();
    auto pixmap = Utils::generateTintedPixmap(":/images/icons/baseline-error_outline-24px.svg", RingTheme::urgentOrange_)
        .scaled(spinnerGeometry.width(), spinnerGeometry.height());
    ui->spinnerLabel->setPixmap(pixmap);
    ui->progressLabel->setStyleSheet("color:red;");
    ui->progressLabel->setText(tr("Error creating account"));
    QTimer::singleShot(1000, this,
        [this] {
            changePage(ui->welcomePage);
            ui->progressLabel->setStyleSheet("");
            ui->spinnerLabel->setMovie(creationSpinnerMovie_);
        });
}
