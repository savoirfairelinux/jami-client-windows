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

    lookupStatusLabel_ = new QLabel(this);
    lookupStatusLabel_->setMovie(lookupSpinnerMovie_);
    lookupStatusLabel_->hide();

    registrationStateOk_ = false;

    passwordStatusLabel_ = new QLabel(this);

    statusSuccessPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-done-24px.svg", RingTheme::presenceGreen_);
    statusInvalidPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-error_outline-24px.svg", RingTheme::urgentOrange_);
    statusErrorPixmap_ = Utils::generateTintedPixmap(":/images/icons/baseline-close-24px.svg", RingTheme::red_);

    ui->infoWidget->hide();
    setNavBarVisibility(false, true);

    lookupTimer_.setSingleShot(true);

    connect(ui->fileImportBtn, &QPushButton::clicked,
        [this] {
            QString filePath;
            filePath = QFileDialog::getOpenFileName(this,
                tr("Open File"),
                QString(),
                tr("Jami archive files (*.gz); All files (*)"));
            fileToImport_ = QDir::toNativeSeparators(filePath);
            if (!fileToImport_.isEmpty()) {
                QFileInfo fi(filePath);
                ui->fileImportBtn->setText(fi.fileName());
            } else {
                ui->fileImportBtn->setText(tr("(None)"));
            }
            validateWizardProgression();
        });

    connect(&lookupTimer_, &QTimer::timeout,
            this, &NewWizardWidget::timeoutNameLookupTimer);

    connect(ui->backButton, &QPushButton::clicked,
        [this] {
            emit NavigationRequested(ScreenEnum::CallScreen);
        });

    connect(ui->confirmPasswordEdit, &QLineEdit::textChanged,
        [this] {
            validateWizardProgression();
        });

    connect(ui->pinEdit, &QLineEdit::textChanged,
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
NewWizardWidget::setToMigrate(AccountInfo* toBeMigrated)
{
    wizardMode_ = WizardMode::MIGRATE;
    changePage(ui->createRingAccountPage);
    ui->usernameEdit->setEnabled(false);
    ui->usernameEdit->setText(QString::fromStdString(toBeMigrated->profileInfo.alias));
    ui->previousButton->hide();
    ui->infoWidget->show();
    ui->infoLabel->setText(tr("Your account needs to be migrated. Enter your password."));
}

void
NewWizardWidget::updateNameRegistrationUi(NameRegistrationUIState state)
{
    switch (state) {
    case NameRegistrationUIState::BLANK:
        lookupStatusLabel_->hide();
        break;
    case NameRegistrationUIState::INVALID:
        lookupStatusLabel_->setPixmap(statusInvalidPixmap_);
        break;
    case NameRegistrationUIState::TAKEN:
        lookupStatusLabel_->setPixmap(statusErrorPixmap_);
        break;
    case NameRegistrationUIState::FREE:
        lookupStatusLabel_->setPixmap(statusSuccessPixmap_);
        break;
    case NameRegistrationUIState::SEARCHING:
        lookupStatusLabel_->setMovie(lookupSpinnerMovie_);
        lookupSpinnerMovie_->stop();
        lookupSpinnerMovie_->start();
        lookupStatusLabel_->show();
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
NewWizardWidget::slotAccountOnBoarded()
{}

void
NewWizardWidget::on_existingPushButton_clicked()
{
    changePage(ui->linkRingAccountPage);
}

void
NewWizardWidget::on_newAccountButton_clicked()
{
    changePage(ui->createRingAccountPage);
}

void
NewWizardWidget::on_newSIPAccountButton_clicked()
{
    changePage(ui->createRingSIPAccountPage);
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
        lookupStatusLabel_->hide();
        passwordStatusLabel_->hide();
    } else if (toPage == ui->createRingAccountPage) {
        ui->usernameEdit->clear();
        ui->passwordEdit->clear();
        ui->confirmPasswordEdit->clear();
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
    } else if (toPage == ui->createRingSIPAccountPage) {
        ui->SIPusernameEdit->clear();
        ui->SIPpasswordEdit->clear();
        ui->SIPservernameEdit->clear();
        ui->SIPproxyEdit->clear();
        ui->SIPfullNameEdit->setText(QString());
        ui->SIPusernameEdit->setEnabled(true);
        setNavBarVisibility(true);
        ui->nextButton->setEnabled(true);
        ui->setSIPAvatarWidget->startBooth();
    } else if (toPage == ui->linkRingAccountPage) {
        fileToImport_ = QString("");
        ui->fileImportBtn->setText(tr("(None)"));
        ui->pinEdit->clear();
        ui->importPasswordEdit->clear();
        ui->pinEdit->setEnabled(true);
        ui->fileImportBtn->setEnabled(true);
        setNavBarVisibility(true);
        lookupStatusLabel_->hide();
        passwordStatusLabel_->hide();
        validateWizardProgression();
    } else if (toPage == ui->spinnerPage) {
        lookupStatusLabel_->hide();
        passwordStatusLabel_->hide();
    }
}

void
NewWizardWidget::updateCustomUI()
{
    QPoint editUsernamePos = ui->usernameEdit->mapTo(this, ui->usernameEdit->rect().topRight());
    lookupStatusLabel_->setGeometry(editUsernamePos.x() + 6, editUsernamePos.y() - 1, 30, 30);
    QPoint editconfpassPos = ui->confirmPasswordEdit->mapTo(this, ui->confirmPasswordEdit->rect().topRight());
    passwordStatusLabel_->setGeometry(editconfpassPos.x() + 6, editconfpassPos.y() - 1, 24, 24);
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
    if (curWidget == ui->createRingAccountPage) {
        wizardMode_ = WizardMode::CREATE;
        processWizardInformations();
    } else if (curWidget == ui->linkRingAccountPage) {
        wizardMode_ = WizardMode::IMPORT;
        processWizardInformations();
    } else if (curWidget == ui->createRingSIPAccountPage) {
        wizardMode_ = WizardMode::CREATESIP;
        processWizardInformations();
    }
}

void
NewWizardWidget::on_previousButton_clicked()
{
    const QWidget* curWidget = ui->stackedWidget->currentWidget();
    if (curWidget == ui->createRingAccountPage) { ui->setAvatarWidget->stopBooth(); }
    if (curWidget == ui->createRingSIPAccountPage) { ui->setSIPAvatarWidget->stopBooth(); }
    disconnect(registeredNameFoundConnection_);
    lookupStatusLabel_->hide();
    passwordStatusLabel_->hide();
    if (curWidget == ui->createRingAccountPage ||
        curWidget == ui->linkRingAccountPage || curWidget == ui->createRingSIPAccountPage) {
        changePage(ui->welcomePage);
    }
}

void
NewWizardWidget::on_passwordEdit_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1);
    validateWizardProgression();
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
        NameDirectory::instance().lookupName(nullptr, QString(), registeredName_);
    }
}

void
NewWizardWidget::slotRegisteredNameFound(const std::string& accountId,
                                         LookupStatus status,
                                         const std::string& address,
                                         const std::string& name)
{
    Q_UNUSED(accountId);
    Q_UNUSED(address);

    using namespace lrc::api::account;
    if (name.length() < 3) {
        registrationStateOk_ = false;
        updateNameRegistrationUi(NameRegistrationUIState::INVALID);
    } else if (registeredName_.toStdString() == name) {
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
    if (ui->stackedWidget->currentWidget() == ui->linkRingAccountPage) {
        bool validPin = !ui->pinEdit->text().isEmpty();
        ui->fileImportBtn->setEnabled(!validPin);
        ui->fileImportLabel->setEnabled(!validPin);
        bool validImport = !fileToImport_.isEmpty();
        ui->pinEdit->setEnabled(!validImport);
        ui->pinEditLabel->setEnabled(!validImport);
        ui->nextButton->setEnabled(validPin || validImport);
        return;
    }
    bool usernameOk =
        !ui->signUpCheckbox->isChecked() ||
        (ui->signUpCheckbox->isChecked() &&
         !registeredName_.isEmpty() &&
         (registeredName_ == ui->usernameEdit->text()) &&
         registrationStateOk_ == true);
    bool passwordOk = ui->passwordEdit->text() == ui->confirmPasswordEdit->text();
    if (passwordOk && !ui->passwordEdit->text().isEmpty()) {
        passwordStatusLabel_->show();
        passwordStatusLabel_->setPixmap(statusSuccessPixmap_);
    } else if (!passwordOk) {
        passwordStatusLabel_->show();
        passwordStatusLabel_->setPixmap(statusErrorPixmap_);
    } else {
        passwordStatusLabel_->hide();
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
        inputPara_["password"] = ui->passwordEdit->text();
        ui->fullNameEdit->clear();
        ui->passwordEdit->clear();
        ui->confirmPasswordEdit->clear();
        break;
    case WizardMode::IMPORT:
        ui->progressLabel->setText(tr("Importing account archive..."));
        inputPara_["password"] = ui->importPasswordEdit->text();
        inputPara_["archivePin"] = ui->pinEdit->text();
        ui->pinEdit->clear();
        ui->importPasswordEdit->clear();
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
    }

    inputPara_["archivePath"] = fileToImport_;
    if (inputPara_.find("archivePin") == inputPara_.end()) {
        inputPara_["archivePin"] = "";
    }

    changePage(ui->spinnerPage);
    createAccount();

    Utils::CreateStartupLink();
}

void
NewWizardWidget::createAccount()
{
    bool isRing = wizardMode_ == WizardMode::CREATE || wizardMode_ == WizardMode::IMPORT;
    Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountAdded,
        [this, isRing](const std::string& accountId) {
            //set default ringtone
            auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
            confProps.Ringtone.ringtonePath = Utils::GetRingtonePath().toStdString();
            if (!isRing) {
                // set SIP details
                confProps.hostname = inputPara_["hostname"].toStdString();
                confProps.username = inputPara_["username"].toStdString();
                confProps.password = inputPara_["password"].toStdString();
                confProps.proxyServer = inputPara_["proxy"].toStdString();
            }
            LRCInstance::accountModel().setAccountConfig(accountId, confProps);
            if (isRing) {
                if (!confProps.username.empty()) {
                    connect(LRCInstance::editableAccountModel(),
                        &lrc::api::NewAccountModel::nameRegistrationEnded,
                        [this] {
                            lrc::api::account::ConfProperties_t accountProperties = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
                            LRCInstance::accountModel().setAccountConfig(LRCInstance::getCurrAccId(), accountProperties);
                            emit NavigationRequested(ScreenEnum::CallScreen);
                            emit LRCInstance::instance().accountOnBoarded();
                        });
                    LRCInstance::editableAccountModel()->registerName(
                        LRCInstance::getCurrAccId(),
                        "",
                        registeredName_.toStdString()
                    );
                } else {
                    emit NavigationRequested(ScreenEnum::CallScreen);
                    emit LRCInstance::instance().accountOnBoarded();
                }
                LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
            } else {
                LRCInstance::setCurrAccAvatar(ui->setSIPAvatarWidget->getAvatarPixmap());

            }
    });
    Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountRemoved,
        [this](const std::string& accountId) {
            Q_UNUSED(accountId);
            qWarning() << Q_FUNC_INFO << ": " << "accountRemoved";
            reportFailure();
        });
    Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::invalidAccountDetected,
        [this](const std::string& accountId) {
            Q_UNUSED(accountId);
            qWarning() << Q_FUNC_INFO << ": " << "invalidAccountDetected";
            reportFailure();
        });
    QtConcurrent::run(
        [this, isRing] {
            if (isRing) {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::RING,
                    inputPara_["alias"].toStdString(),
                    inputPara_["archivePath"].toStdString(),
                    inputPara_["password"].toStdString(),
                    inputPara_["archivePin"].toStdString()
                );
            } else {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::SIP,
                    inputPara_["alias"].toStdString(),
                    inputPara_["archivePath"].toStdString(),
                    "",
                    "",
                    inputPara_["username"].toStdString()
                );
                QThread::sleep(2);
                emit NavigationRequested(ScreenEnum::CallScreen);
                emit LRCInstance::instance().accountOnBoarded();
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
