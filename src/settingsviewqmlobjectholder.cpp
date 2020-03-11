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

#include "settingsviewqmlobjectholder.h"

SettingsViewQmlObjectHolder::SettingsViewQmlObjectHolder(QObject *parent) : QObject(parent)
{

}

Q_INVOKABLE void
SettingsViewQmlObjectHolder::setSettingsViewQmlObject(QObject *obj)
{
    settingsViewQmlObject = obj;
}

QVariantMap
SettingsViewQmlObjectHolder::getConfProps()
{
    return QVariantMap();
}

void
SettingsViewQmlObjectHolder::setConfProps(QVariantMap propInMap)
{}

void
SettingsViewQmlObjectHolder::setRegisteredName(QString registeredName)
{}

void
SettingsViewQmlObjectHolder::setSelectedMenu(SettingsMenu menu)
{
    if (selectedMenu_ != menu) {
        selectedMenu_ = menu;
        emit selectedMenuChanged();
    }
}

Q_INVOKABLE bool
SettingsViewQmlObjectHolder::getIsSIP()
{
    return LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::SIP;
}

Q_INVOKABLE void
SettingsViewQmlObjectHolder::startPreviewing(bool force)
{
    LRCInstance::renderer()->startPreviewing(force);
}

Q_INVOKABLE void
SettingsViewQmlObjectHolder::stopPreviewing()
{
    if (!LRCInstance::hasVideoCall()) {
        LRCInstance::renderer()->stopPreviewing();
    }
}

Q_INVOKABLE void
SettingsViewQmlObjectHolder::setHardwareAccel(bool state)
{
    LRCInstance::accountModel().setAccountEnabled(LRCInstance::getCurrAccId(),
                                                  static_cast<bool>(state));
}

Q_INVOKABLE bool
SettingsViewQmlObjectHolder::validateRegNameForm(const QString &regName)
{
    QRegularExpression regExp(" ");

    if (regName.size() > 2 && !regName.contains(regExp)) {
        return true;

    } else {
        return false;
    }
}

void
SettingsViewQmlObjectHolder::afterNameLookup(lrc::api::account::LookupStatus status,
                                             const QString &regName)
{
    if (registeredName_ == regName && regName.length() > 2) {
        if (status == lrc::api::account::LookupStatus::NOT_FOUND) {
            emit setRegNameUi(RegName::FREE);

        } else {
            emit setRegNameUi(RegName::TAKEN);
        }

    } else {
        emit setRegNameUi(RegName::BLANK);
    }
}
