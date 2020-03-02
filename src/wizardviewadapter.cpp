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

#include "wizardviewadapter.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#undef REGISTERED
#include "../daemon/src/dring/account_const.h"

WizardViewAdapter::WizardViewAdapter(QObject *parent)
    : QObject(parent)
{}

void
WizardViewAdapter::setWizardViewAdapter(QObject *obj)
{
    wizardViewAdapter = obj;
}

void
WizardViewAdapter::settingsNeverShowAgainBox(bool checked)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::neverShowMeAgain, checked);
}

bool
WizardViewAdapter::exportToFile(const QString accountId,
                                           const QString path,
                                           const QString password)
{
    return LRCInstance::accountModel().exportToFile(accountId,path, password);
}

bool
WizardViewAdapter::hasPassword()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    return confProps.archiveHasPassword;
}

void
WizardViewAdapter::createJamiAccount(const QVariantMap &settings, bool isCreating)
{
    Utils::oneShotConnect(
        &LRCInstance::accountModel(),
        &lrc::api::NewAccountModel::accountAdded,
            [this, settings, isCreating](const QString &accountId) {
                QSettings qSettings("jami.net", "Jami");
                if (not qSettings.contains(SettingsKey::neverShowMeAgain)) {
                    qSettings.setValue(SettingsKey::neverShowMeAgain, false);
                }
                auto showBackup = isCreating && !settings.value(SettingsKey::neverShowMeAgain).toBool();

                qDebug() << "The showbackup in C++ is: " + showBackup;

                if (!settings["registeredName"].toString().isEmpty()) {
                    Utils::oneShotConnect(&LRCInstance::accountModel(),
                                          &lrc::api::NewAccountModel::nameRegistrationEnded,
                                          [this, showBackup] {
                                                emit accountAdded(showBackup);
                                          });
                    LRCInstance::accountModel().registerName(LRCInstance::getCurrAccId(),
                                                             "",
                                                             settings["registeredName"].toString());
                } else {
                    emit accountAdded(showBackup);
                }
                //TODO: set up avatar pixmap from photobooth
                //LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
    });

    connectFailure();

    QtConcurrent::run([this,settings] {
        QMap<QString, QString> additionalAccountConfig;
        additionalAccountConfig.insert(DRing::Account::ConfProperties::Ringtone::PATH,
                                       Utils::GetRingtonePath());

            LRCInstance::accountModel().createNewAccount(lrc::api::profile::Type::RING,
                                                         settings["alias"].toString(),
                                                         settings["archivePath"].toString(),
                                                         settings["password"].toString(),
                                                         settings["archivePin"].toString(),
                                                         "",
                                                         additionalAccountConfig);
        });
}

void
WizardViewAdapter::createSIPAccount(const QVariantMap &settings)
{
    Utils::oneShotConnect(
        &LRCInstance::accountModel(),
        &lrc::api::NewAccountModel::accountAdded,
            [this, settings](const QString &accountId) {
            auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
            // set SIP details
            confProps.hostname = settings["hostname"].toString();
            confProps.username = settings["username"].toString();
            confProps.password = settings["password"].toString();
            confProps.proxyServer = settings["proxy"].toString();
            LRCInstance::accountModel().setAccountConfig(accountId, confProps);

            // TODO: set up photobooth avatar to SIP avatar
            //LRCInstance::setCurrAccAvatar(ui->setSIPAvatarWidget->getAvatarPixmap());
    });

    connectFailure();

    QtConcurrent::run([this, settings] {
        QMap<QString, QString> additionalAccountConfig;
        additionalAccountConfig.insert(DRing::Account::ConfProperties::Ringtone::PATH,
                                       Utils::GetRingtonePath());

        LRCInstance::accountModel().createNewAccount(lrc::api::profile::Type::SIP,
                                                     settings["alias"].toString(),
                                                     settings["archivePath"].toString(),
                                                     "",
                                                     "",
                                                     settings["username"].toString(),
                                                     additionalAccountConfig);
        QThread::sleep(2);
        emit showMainViewWindow();
        emit LRCInstance::instance().accountListChanged();

    });
}

void
WizardViewAdapter::createJAMSAccount(const QVariantMap &settings)
{
    Utils::oneShotConnect(&LRCInstance::accountModel(),
                          &lrc::api::NewAccountModel::accountAdded,
                          [this](const QString &accountId) {
                              Q_UNUSED(accountId)
                              if (!LRCInstance::accountModel().getAccountList().size())
                                  return;
                              emit showMainViewWindow();
                              emit LRCInstance::instance().accountListChanged();
                          });

    connectFailure();

    QtConcurrent::run([this, settings] {
        QMap<QString, QString> additionalAccountConfig;
        additionalAccountConfig.insert(DRing::Account::ConfProperties::Ringtone::PATH,
                                       Utils::GetRingtonePath());

        LRCInstance::accountModel().connectToAccountManager(settings["username"].toString(),
                                                            settings["password"].toString(),
                                                            settings["manager"].toString(),
                                                            additionalAccountConfig);

    });
}

void
WizardViewAdapter::connectFailure()
{
    Utils::oneShotConnect(&LRCInstance::accountModel(),
                          &lrc::api::NewAccountModel::accountRemoved,
                          [this](const QString &accountId) {
                              Q_UNUSED(accountId);

                              emit reportFailure();
                          });
    Utils::oneShotConnect(&LRCInstance::accountModel(),
                          &lrc::api::NewAccountModel::invalidAccountDetected,
                          [this](const QString &accountId) {
                              Q_UNUSED(accountId);

                              emit reportFailure();
                          });
}

void
WizardViewAdapter::printSettingParaMap(const QVariantMap &map)
{
    for (auto pair : map.toStdMap()) {
        qDebug() << "Printing para setting map, Key: " + pair.first
                        + " , value: " + pair.second.toString();
    }
}
