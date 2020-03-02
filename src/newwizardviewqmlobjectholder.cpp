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
#include <QVariantMap>
#include <QtConcurrent/QtConcurrent>

#undef REGISTERED
#include "../daemon/src/dring/account_const.h"

NewWizardViewQmlObjectHolder::NewWizardViewQmlObjectHolder(QObject *parent)
    : QObject(parent)
{}

void
NewWizardViewQmlObjectHolder::setNewWizardViewQmlObject(QObject *obj)
{
    newWizardViewQmlObject = obj;
}

void
NewWizardViewQmlObjectHolder::settingsNeverShowAgainBox(bool checked)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::neverShowMeAgain, checked);
}

bool
NewWizardViewQmlObjectHolder::exportToFile(const QString accountId,
                                           const QString path,
                                           const QString password)
{
    return LRCInstance::accountModel().exportToFile(accountId,path, password);
}

bool
NewWizardViewQmlObjectHolder::hasPassword()
{
    auto confProps = LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
    return confProps.archiveHasPassword;
}

void
NewWizardViewQmlObjectHolder::createJamiAccount(const QVariantMap &settings, bool isCreating)
{
    //Utils::oneShotConnect(
    //    &LRCInstance::accountModel(),
    //    &lrc::api::NewAccountModel::accountAdded,
    //        [this, settings, isCreating](const QString &accountId) {
    //            QSettings qSettings("jami.net", "Jami");
    //            if (not qSettings.contains(SettingsKey::neverShowMeAgain)) {
    //                qSettings.setValue(SettingsKey::neverShowMeAgain, false);
    //            }
    //            auto showBackup = isCreating && !settings.value(SettingsKey::neverShowMeAgain).toBool();

    //            if (!settings["registeredName"].toString().isEmpty()) {
    //                Utils::oneShotConnect(&LRCInstance::accountModel(),
    //                                      &lrc::api::NewAccountModel::nameRegistrationEnded,
    //                                      [this, showBackup] {
    //                                            emit accountAdded(showBackup);
    //                                      });
    //                LRCInstance::accountModel().registerName(LRCInstance::getCurrAccId(),
    //                                                         "",
    //                                                         settings["registeredName"].toString());
    //            } else {
    //                emit accountAdded(showBackup);
    //            }
    //            //TODO: set up avatar pixmap from photobooth
    //            //LRCInstance::setCurrAccAvatar(ui->setAvatarWidget->getAvatarPixmap());
    //    });

    //connectFailure();

    //QtConcurrent::run([this,settings] {
    //    QMap<QString, QString> additionalAccountConfig;
    //    additionalAccountConfig.insert(DRing::Account::ConfProperties::Ringtone::PATH,
    //                                   Utils::GetRingtonePath());

    //        LRCInstance::accountModel().createNewAccount(lrc::api::profile::Type::RING,
    //                                                 settings["alias"].toString(),
    //                                                 settings["archivePath"].toString(),
    //                                                 settings["password"].toString(),
    //                                                 settings["archivePin"].toString(),
    //                                                     "",
    //                                                     additionalAccountConfig);
    //    });

    qDebug() << "Creating Jami account";
    printSettingParaMap(settings);
}

void
NewWizardViewQmlObjectHolder::createSIPAccount(const QVariantMap &settings)
{
    qDebug() << "Creating SIP account";
    printSettingParaMap(settings);

}

void
NewWizardViewQmlObjectHolder::createJAMSAccount(const QVariantMap &settings)
{
    qDebug() << "Creating JSMS account";
    printSettingParaMap(settings);
}

void
NewWizardViewQmlObjectHolder::connectFailure()
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
NewWizardViewQmlObjectHolder::printSettingParaMap(const QVariantMap &map)
{
    for (auto pair : map.toStdMap()) {
        qDebug() << "Printing para setting map, Key: " + pair.first
                        + " , value: " + pair.second.toString();
    }
}
