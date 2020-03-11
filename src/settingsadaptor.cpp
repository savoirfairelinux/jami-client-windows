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
