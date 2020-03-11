/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settingsadaptor.h"

#include "api/newdevicemodel.h"

SettingsAdaptor::SettingsAdaptor(QObject *parent)
    : QObject(parent)
{}

QString
SettingsAdaptor::getDir_Document()
{
    return QDir::toNativeSeparators(
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
}

QString
SettingsAdaptor::getDir_Download()
{
    QString downloadPath = QDir::toNativeSeparators(
        LRCInstance::dataTransferModel().downloadDirectory);
    int pos = downloadPath.lastIndexOf(QChar('\\'));
    if (pos == downloadPath.length() - 1) {
        downloadPath.truncate(pos);
    }
    return downloadPath;
}

bool
SettingsAdaptor::getSettingsValue_CloseOrMinimized()
{
    QSettings settings("jami.net", "Jami");
    return settings.value(SettingsKey::closeOrMinimized).toBool();
}

bool
SettingsAdaptor::getSettingsValue_EnableNotifications()
{
    QSettings settings("jami.net", "Jami");
    return settings.value(SettingsKey::enableNotifications).toBool();
}

bool
SettingsAdaptor::getSettingsValue_AutoUpdate()
{
    QSettings settings("jami.net", "Jami");
    return settings.value(SettingsKey::autoUpdate).toBool();
}

void
SettingsAdaptor::setClosedOrMin(bool state)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::closeOrMinimized, state);
}

void
SettingsAdaptor::setNotifications(bool state)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::enableNotifications, state);
}

void
SettingsAdaptor::setUpdateAutomatic(bool state)
{
#ifdef Q_OS_WIN
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::autoUpdate, state);
#endif
}

void
SettingsAdaptor::setRunOnStartUp(bool state)
{
    if (Utils::CheckStartupLink(L"Jami")) {
        if (!state) {
            Utils::DeleteStartupLink(L"Jami");
        }
    } else if (state) {
        Utils::CreateStartupLink(L"Jami");
    }
}

void
SettingsAdaptor::setDownloadPath(QString dir)
{
    QSettings settings("jami.net", "Jami");
    settings.setValue(SettingsKey::downloadPath, dir);
    LRCInstance::dataTransferModel().downloadDirectory = dir + "/";
}

lrc::api::video::ResRateList
SettingsAdaptor::get_ResRateList(lrc::api::video::Channel channel, QString device)
{
    auto deviceCapabilities = get_DeviceCapabilities(device);

    return deviceCapabilities[channel];
}

int
SettingsAdaptor::get_DeviceCapabilitiesSize(const QString &device)
{
    return get_DeviceCapabilities(device).size();
}

QVector<QString>
SettingsAdaptor::getResolutions(const QString &device)
{
    QVector<QString> resolutions;

    auto currentSettings = LRCInstance::avModel().getDeviceSettings(device);

    auto currentChannel = currentSettings.channel.isEmpty() ? "default" : currentSettings.channel;
    auto channelCaps = get_ResRateList(currentChannel, device);
    for (auto [resolution, frameRateList] : channelCaps) {
        for (auto rate : frameRateList) {
            resolutions.append(resolution);
        }
    }

    return resolutions;
}

QVector<int>
SettingsAdaptor::getFrameRates(const QString &device)
{
    QVector<int> rates;

    auto currentSettings = LRCInstance::avModel().getDeviceSettings(device);

    auto currentChannel = currentSettings.channel.isEmpty() ? "default" : currentSettings.channel;
    auto channelCaps = get_ResRateList(currentChannel, device);
    for (auto [resolution, frameRateList] : channelCaps) {
        for (auto rate : frameRateList) {
            rates.append((int) rate);
        }
    }

    return rates;
}

lrc::api::video::Capabilities
SettingsAdaptor::get_DeviceCapabilities(const QString &device)
{
    return LRCInstance::avModel().getDeviceCapabilities(device);
}

QString
SettingsAdaptor::get_Video_Settings_Channel(const QString &deviceId)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);

    return (QString) settings.channel;
}

QString
SettingsAdaptor::get_Video_Settings_Name(const QString &deviceId)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);

    return (QString) settings.name;
}

QString
SettingsAdaptor::get_Video_Settings_Id(const QString &deviceId)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);

    return (QString) settings.id;
}

qreal
SettingsAdaptor::get_Video_Settings_Rate(const QString &deviceId)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);

    return (qreal) settings.rate;
}

QString
SettingsAdaptor::get_Video_Settings_Size(const QString &deviceId)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);

    return (QString) settings.size;
}

void
SettingsAdaptor::set_Video_Settings_Rate_And_Resolution(const QString &deviceId,
                                                        qreal rate,
                                                        const QString &resolution)
{
    auto settings = LRCInstance::avModel().getDeviceSettings(deviceId);
    settings.rate = rate;
    settings.size = resolution;
    LRCInstance::avModel().setDeviceSettings(settings);
}

const lrc::api::account::Info &
SettingsAdaptor::getCurrentAccountInfo()
{
    return LRCInstance::getCurrentAccountInfo();
}

const Q_INVOKABLE lrc::api::profile::Info &
SettingsAdaptor::getCurrentAccount_Profile_Info()
{
    return LRCInstance::getCurrentAccountInfo().profileInfo;
}

ContactModel *
SettingsAdaptor::getContactModel()
{
    return getCurrentAccountInfo().contactModel.get();
}

NewDeviceModel *
SettingsAdaptor::getDeviceModel()
{
    return getCurrentAccountInfo().deviceModel.get();
}

QString
SettingsAdaptor::get_CurrentAccountInfo_RegisteredName()
{
    return LRCInstance::getCurrentAccountInfo().registeredName;
}

bool
SettingsAdaptor::get_CurrentAccountInfo_Enabled()
{
    return LRCInstance::getCurrentAccountInfo().enabled;
}

QString
SettingsAdaptor::getCurrentAccount_Profile_Info_Uri()
{
    return getCurrentAccount_Profile_Info().uri;
}

QString
SettingsAdaptor::getCurrentAccount_Profile_Info_Alias()
{
    return getCurrentAccount_Profile_Info().alias;
}

int
SettingsAdaptor::getCurrentAccount_Profile_Info_Type()
{
    return (int) (getCurrentAccount_Profile_Info().type);
}

QString
SettingsAdaptor::getAvatarImage_Base64(int avatarSize)
{
    auto &accountInfo = LRCInstance::getCurrentAccountInfo();
    auto avatar = Utils::accountPhoto(accountInfo, {avatarSize, avatarSize});

    return QString::fromLatin1(Utils::QImageToByteArray(avatar).toBase64().data());
}

bool
SettingsAdaptor::getIsDefaultAvatar()
{
    auto &accountInfo = LRCInstance::getCurrentAccountInfo();

    return accountInfo.profileInfo.avatar.isEmpty();
}

bool
SettingsAdaptor::setCurrAccAvatar(QString avatarImgBase64)
{
    QImage avatarImg;
    const bool ret = avatarImg.loadFromData(QByteArray::fromBase64(avatarImgBase64.toLatin1()));
    if (!ret) {
        qDebug() << "Current avatar loading from base64 fail";
        return false;
    } else {
        LRCInstance::setCurrAccAvatar(QPixmap::fromImage(avatarImg));
    }
    return true;
}

void
SettingsAdaptor::clearCurrentAvatar()
{
    LRCInstance::setCurrAccAvatar(QPixmap());
}

lrc::api::account::ConfProperties_t
SettingsAdaptor::getAccountConfig()
{
    return LRCInstance::accountModel().getAccountConfig(LRCInstance::getCurrAccId());
}

QString
SettingsAdaptor::getAccountConfig_Manageruri()
{
    return getAccountConfig().managerUri;
}

QString
SettingsAdaptor::getAccountConfig_Username()
{
    return getAccountConfig().username;
}

QString
SettingsAdaptor::getAccountConfig_Hostname()
{
    return getAccountConfig().hostname;
}

QString
SettingsAdaptor::getAccountConfig_Password()
{
    return getAccountConfig().password;
}

QString
SettingsAdaptor::getAccountConfig_ProxyServer()
{
    return getAccountConfig().proxyServer;
}
