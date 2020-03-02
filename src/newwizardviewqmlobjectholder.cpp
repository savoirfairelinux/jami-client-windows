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

#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>

NewWizardViewQmlObjectHolder::NewWizardViewQmlObjectHolder(QObject *parent)
    : QObject(parent)
{
    setRegistrationStateOk(false);

  /*  connect(this, &NewWizardViewQmlObjectHolder::inputParaChanged, [this] { qDebug() << "the input para is changed";
        for (auto pair : inputPara_) {
            qDebug() << "Input Para: Key: " + pair.first + " Value: " + inputPara_[pair.first];
        }
        });*/
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

void
NewWizardViewQmlObjectHolder::setInputPara(QVariantMap para)
{
    qDebug() << "setting input para";

    inputPara_.clear();
    for (auto key : para.keys()) {
        inputPara_[key] = para[key].toString();

        qDebug() << "Setting input para, Key: " + key + " Value: " + para[key].toString();
    }
    emit inputParaChanged(para);
}

QVariantMap
NewWizardViewQmlObjectHolder::getInputPara()
{
    QVariantMap output;
    for (auto pair : inputPara_) {
        output.insert(pair.first, QVariant(inputPara_[pair.first]));
    }

    return output;
}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::setNewWizardViewQmlObject(QObject *obj)
{
    newWizardViewQmlObject = obj;
}

//Q_INVOKABLE void
//NewWizardViewQmlObjectHolder::setInputParaHashPair(QString key, QString value)
//{
//    inputPara_[key] = value;
//}

//Q_INVOKABLE bool
//NewWizardViewQmlObjectHolder::getIfKeyExistInParaHashPair(QString key)
//{
//    return (inputPara_.find(key) == inputPara_.end());
//}

Q_INVOKABLE void
NewWizardViewQmlObjectHolder::createAccount()
{
    bool isConnectingToManager = wizardMode_ == WizardMode::CONNECTMANAGER;
    bool isRing = wizardMode_ == WizardMode::CREATE || wizardMode_ == WizardMode::IMPORT;
    bool isCreating = wizardMode_ == WizardMode::CREATE;
    if (isConnectingToManager) {
        Utils::oneShotConnect(&LRCInstance::accountModel(),
                              &lrc::api::NewAccountModel::accountAdded,
                              [this](const QString &accountId) {
                                  if (!LRCInstance::accountModel().getAccountList().size())
                                      return;
                                  //set default ringtone
                                  auto confProps = LRCInstance::accountModel().getAccountConfig(
                                      accountId);
                                  confProps.Ringtone.ringtonePath = Utils::GetRingtonePath();
                                  LRCInstance::accountModel().setAccountConfig(accountId, confProps);
                                  emit showMainViewWindow();
                                  emit LRCInstance::instance().accountListChanged();
                              });
    } else {
        Utils::oneShotConnect(
            &LRCInstance::accountModel(),
            &lrc::api::NewAccountModel::accountAdded,
            [this, isRing, isCreating](const QString &accountId) {
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
                    auto showBackup = isCreating
                                      && !settings.value(SettingsKey::neverShowMeAgain).toBool();

                    if (!confProps.username.isEmpty()) {
                        Utils::oneShotConnect(
                            &LRCInstance::accountModel(),
                            &lrc::api::NewAccountModel::nameRegistrationEnded,
                            [this, showBackup] {
                                lrc::api::account::ConfProperties_t accountProperties
                                    = LRCInstance::accountModel().getAccountConfig(
                                        LRCInstance::getCurrAccId());
                                LRCInstance::accountModel()
                                    .setAccountConfig(LRCInstance::getCurrAccId(),
                                                      accountProperties);
                                QSettings settings("jami.net", "Jami");
                                if (showBackup) {
                                    emit changePage(4);
                                } else {
                                    emit showMainViewWindow();
                                    emit LRCInstance::instance().accountListChanged();
                                }
                            });
                        LRCInstance::accountModel().registerName(LRCInstance::getCurrAccId(),
                                                                 "",
                                                                 registeredName_);
                    } else {
                        if (showBackup) {
                            emit changePage(4);
                        } else {
                            emit showMainViewWindow();
                            emit LRCInstance::instance().accountListChanged();
                        }
                    }
                    //TODO: set up avatar pixmap from photobooth
                    //LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
                } else {
                    //TODO: set up avatar pixmap from photobooth
                    //LRCInstance::setCurrAccAvatar(ui->setSIPAvatarWidget->getAvatarPixmap());
                }
            });
    }
    Utils::oneShotConnect(&LRCInstance::accountModel(),
                          &lrc::api::NewAccountModel::accountRemoved,
                          [this](const QString &accountId) {
                              Q_UNUSED(accountId);
                              qWarning() << Q_FUNC_INFO << ": "
                                         << "accountRemoved";
                              reportFailure();
                          });
    Utils::oneShotConnect(&LRCInstance::accountModel(),
                          &lrc::api::NewAccountModel::invalidAccountDetected,
                          [this](const QString &accountId) {
                              Q_UNUSED(accountId);
                              qWarning() << Q_FUNC_INFO << ": "
                                         << "invalidAccountDetected";
                              reportFailure();
                          });
    QtConcurrent::run([this, isRing, isConnectingToManager] {
        if (isConnectingToManager) {
            LRCInstance::accountModel().connectToAccountManager(inputPara_["username"],
                                                                inputPara_["password"],
                                                                inputPara_["manager"]);
        } else if (isRing) {
            LRCInstance::accountModel().createNewAccount(lrc::api::profile::Type::RING,
                                                         inputPara_["alias"],
                                                         inputPara_["archivePath"],
                                                         inputPara_["password"],
                                                         inputPara_["archivePin"]);
        } else {
            LRCInstance::accountModel().createNewAccount(lrc::api::profile::Type::SIP,
                                                         inputPara_["alias"],
                                                         inputPara_["archivePath"],
                                                         "",
                                                         "",
                                                         inputPara_["username"]);
            QThread::sleep(2);
            emit showMainViewWindow();
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
