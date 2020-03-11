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

const lrc::api::account::Info &
SettingsAdaptor::getCurrentAccountInfo()
{
    return LRCInstance::getCurrentAccountInfo();
}

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
SettingsAdaptor::get_DeviceCapabilitiesSize(QString device)
{
    return get_DeviceCapabilities(device).size();
}

QVector<QString>
SettingsAdaptor::getResolutions(QString device)
{
    QVector<QString> resolutions;

    auto currentSettings = getDeviceSettings(device);

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
SettingsAdaptor::getFrameRates(QString device)
{
    QVector<int> rates;

    auto currentSettings = getDeviceSettings(device);

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
SettingsAdaptor::get_DeviceCapabilities(QString device)
{
    return LRCInstance::avModel().getDeviceCapabilities(device);
}

lrc::api::video::Settings
SettingsAdaptor::getDeviceSettings(QString device)
{
    return LRCInstance::avModel().getDeviceSettings(device);
}

QString
SettingsAdaptor::getChannel_DeviceSettings(QString device)
{
    auto deviceSettings = getDeviceSettings(device);

    return (QString) deviceSettings.channel;
}
