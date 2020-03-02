/***************************************************************************
* Copyright (C) 2019-2020 by Savoir-faire Linux                            *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                       *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/

#include "newwizardviewqmlobjectholder.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>

NewWizardViewQmlObjectHolder::NewWizardViewQmlObjectHolder(QObject* parent)
    : QObject(parent)
{
    setRegistrationStateOk(false);
}

void
NewWizardViewQmlObjectHolder::setRegistrationStateOk(bool ok)
{
    if (registrationStateOk_ != ok) {
        registrationStateOk_ = ok;
        emit registrationStateOkChanged(ok);
    }
}

void
NewWizardViewQmlObjectHolder::setFileToImport(QString para_fileToImport)
{
    if (fileToImport_ != para_fileToImport) {
        fileToImport_ = para_fileToImport;
        emit fileToImportChanged(para_fileToImport);
    }
}

void
NewWizardViewQmlObjectHolder::setRegisteredName(QString para_registeredName)
{
    if (registeredName_ != para_registeredName) {
        registeredName_ = para_registeredName;
        emit registeredNameChanged(para_registeredName);
    }
}

void
NewWizardViewQmlObjectHolder::setWizardMode(WizardMode mode)
{
    if (wizardMode_ != mode) {
        wizardMode_ = mode;
        emit wizardModeChanged(mode);
    }
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setNewWizardViewQmlObject(QObject* obj)
{
    newWizardViewQmlObject = obj;
}

Q_INVOKABLE int
NewWizardViewQmlObjectHolder::getAccountListSize()
{
    return LRCInstance::accountModel().getAccountList().size();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::clearInputPara()
{
    inputPara_.clear();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setInputParaHashPair(QString key, QString value)
{
    inputPara_[key] = value;
}

Q_INVOKABLE bool
NewWizardViewQmlObjectHolder::getIfKeyExistInParaHashPair(QString key)
{
    return(inputPara_.find(key) == inputPara_.end());
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::createAccount()
{
    bool isConnectingToManager = wizardMode_ == WizardMode::CONNECTMANAGER;
    bool isRing = wizardMode_ == WizardMode::CREATE || wizardMode_ == WizardMode::IMPORT;
    bool isCreating = wizardMode_ == WizardMode::CREATE;
    if (isConnectingToManager) {
        Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountAdded,
            [this](const QString& accountId) {
                if (!LRCInstance::accountModel().getAccountList().size())
                    return;
                //set default ringtone
                auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
                confProps.Ringtone.ringtonePath = Utils::GetRingtonePath();
                LRCInstance::accountModel().setAccountConfig(accountId, confProps);
                //emit NavigationRequested(ScreenEnum::CallScreen);
                emit LRCInstance::instance().accountListChanged();
            });
    }
    else {
        Utils::oneShotConnect(&LRCInstance::accountModel(), &lrc::api::NewAccountModel::accountAdded,
            [this, isRing, isCreating](const QString& accountId) {
                //set default ringtone
                auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
                confProps.Ringtone.ringtonePath = Utils::GetRingtonePath();
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
                                    changePage(4);
                                }
                                else {
                                    //emit NavigationRequested(ScreenEnum::CallScreen);
                                    emit LRCInstance::instance().accountListChanged();
                                }
                            });
                        LRCInstance::accountModel().registerName(
                            LRCInstance::getCurrAccId(),
                            "",
                            registeredName_
                            );
                    }
                    else {
                        if (showBackup) {
                            changePage(4);
                        }
                        else {
                            //emit NavigationRequested(ScreenEnum::CallScreen);
                            emit LRCInstance::instance().accountListChanged();
                        }
                    }
                    //TODO: set up avatar pixmap from photobooth
                    //LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
                }
                else {
                    //TODO: set up avatar pixmap from photobooth
                    //LRCInstance::setCurrAccAvatar(ui->setSIPAvatarWidget->getAvatarPixmap());
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
            if (isConnectingToManager) {
                LRCInstance::accountModel().connectToAccountManager(
                    inputPara_["username"],
                    inputPara_["password"],
                    inputPara_["manager"]
                    );
            }
            else if (isRing) {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::RING,
                    inputPara_["alias"],
                    inputPara_["archivePath"],
                    inputPara_["password"],
                    inputPara_["archivePin"]
                    );
            }
            else {
                LRCInstance::accountModel().createNewAccount(
                    lrc::api::profile::Type::SIP,
                    inputPara_["alias"],
                    inputPara_["archivePath"],
                    "",
                    "",
                    inputPara_["username"]
                    );
                QThread::sleep(2);
                //emit NavigationRequested(ScreenEnum::CallScreen);
                emit LRCInstance::instance().accountListChanged();
            }
        });
    emit changePage(7);
    emit updateQuickWindow();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::settingsNeverShowAgainBox(bool checked)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::neverShowMeAgain, checked);
}

Q_INVOKABLE QString
NewWizardViewQmlObjectHolder::stringSimplifier(QString input)
{
    return input.simplified();
}

Q_INVOKABLE QString
NewWizardViewQmlObjectHolder::toNativeSeparators(QString inputDir)
{
    return QDir::toNativeSeparators(inputDir);
}

Q_INVOKABLE QString
NewWizardViewQmlObjectHolder::toFileInfoName(QString inputFileName)
{
    QFileInfo fi(inputFileName);
    return fi.fileName();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setRegisteredNameFoundSignalConnection(bool toBeConnect)
{
    if (toBeConnect) {
        registeredNameFoundConnection_ = connect(
            &LRCInstance::accountModel(), &lrc::api::NewAccountModel::registeredNameFound,
            this, &NewWizardViewQmlObjectHolder::slotRegisteredNameFound);
    } else {
        disconnect(registeredNameFoundConnection_);
    }
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::exportBtn_clicked(QString fileDir)
{
    if (!fileDir.isEmpty()) {
        LRCInstance::accountModel().exportToFile(LRCInstance::getCurrAccId(), fileDir + "/export.gz");
    }
    //emit NavigationRequested(ScreenEnum::CallScreen);
    emit LRCInstance::instance().accountListChanged();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::skipBtn_clicked()
{
    //emit NavigationRequested(ScreenEnum::CallScreen);
    emit LRCInstance::instance().accountListChanged();
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::timeoutNameLookupTimer(const QString& nameServiceURL, const QString& name)
{
    NameDirectory::instance().lookupName(nameServiceURL, name);
}

void
NewWizardViewQmlObjectHolder::slotRegisteredNameFound(const QString& accountId,
                                                      LookupStatus status,
                                                      const QString& address,
                                                      const QString& name)
{
    Q_UNUSED(accountId);
    Q_UNUSED(address);

    using namespace lrc::api::account;
    if (name.length() < 3) {
        setRegistrationStateOk(false);
        emit updateNameRegistrationUi(NameRegistrationUIState::INVALID);
    }
    else if (getRegisteredName() == name) {
        switch (status) {
        case LookupStatus::NOT_FOUND:
        case LookupStatus::ERROR:
            setRegistrationStateOk(true);
            emit updateNameRegistrationUi(NameRegistrationUIState::FREE);
            break;
        case LookupStatus::INVALID_NAME:
        case LookupStatus::INVALID:
            setRegistrationStateOk(false);
            emit updateNameRegistrationUi(NameRegistrationUIState::INVALID);
            break;
        case LookupStatus::SUCCESS:
            setRegistrationStateOk(false);
            emit updateNameRegistrationUi(NameRegistrationUIState::TAKEN);
            break;
        }
    }
    emit validateWizardProgression();
}
