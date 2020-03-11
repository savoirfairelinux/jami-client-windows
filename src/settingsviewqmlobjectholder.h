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

#pragma once

#include <QObject>

#include "lrcinstance.h"
#include "utils.h"

#include "bannedlistmodel.h"

#include "api/datatransfermodel.h"
#include "typedefs.h"

class SettingsViewQmlObjectHolder : public QObject
{
    Q_OBJECT
public:
    enum SettingsMenu { Account, General, Media };
    enum RegName { BLANK, INVALIDFORM, TAKEN, FREE, SEARCHING };
    Q_ENUM(SettingsMenu)
    Q_ENUM(RegName)

public:
    explicit SettingsViewQmlObjectHolder(QObject *parent = nullptr);

public:
    // functions to call in QML
    Q_INVOKABLE void setSettingsViewQmlObject(QObject *obj);

    // function wrappers for LRC
    Q_INVOKABLE bool getIsSIP();

    // set up variables in current class
    Q_INVOKABLE void setHardwareAccel(bool state);

    // regular utils function wrapper for qml


    // function wrappers for for handling signals and slots


signals:
    // show main view signal
    void showMainViewWindow();

    // property change notification signal

    // object holder signals
    void setRegNameUi(RegName stat);

    // redirect slots
    void leaveSettingsSlot_Redirect();
    void verifyRegisteredNameSlot_Redirect();
    void beforeNameLookup_Redirect();
    void slotRegisterName_Redirect();
    void setAccEnableSlot_Redirect(int state);
    void delAccountSlot_Redirect();
    void toggleAdvancedSIPSettings_Redirect();
    void toggleAdvancedSettings_Redirect();
    void toggleBannedContacts_Redirect();
    void exportAccountSlot_Redirect();
    void updateAndShowDevicesSlot_Redirect();
    void updateAndShowBannedContactsSlot_Redirect();
    void showLinkDevSlot_Redirect();
    void slotSetNotifications_Redirect(bool state);
    void checkForUpdateSlot_Redirect();
    void slotSetClosedOrMin_Redirect(bool state);
    void slotSetRunOnStartUp_Redirect(bool state);
    void openDownloadFolderSlot_Redirect();
    void openRecordFolderSlot_Redirect();
    void slotAlwaysRecordingClicked_Redirect(bool state);
    void slotRecordPreviewClicked_Redirect(bool state);
    void slotRecordQualitySliderValueChanged_Redirect(int value);
    void slotRecordQualitySliderSliderReleased_Redirect();
    void slotSetUpdateAutomatic_Redirect(bool state);
    void slotAudioOutputIndexChanged_Redirect(int index);
    void slotAudioInputIndexChanged_Redirect(int index);
    void slotDeviceBoxCurrentIndexChanged_Redirect(int index);
    void slotFormatBoxCurrentIndexChanged_Redirect(int index);
    void slotVideoDeviceListChanged_Redirect();
    void slotAudioMeter_Redirect(const QString &id, float level);

private slots:
    void receiveRegNameSlot(const QString &accountID,
                            lrc::api::account::LookupStatus status,
                            const QString &address,
                            const QString &name);

private:
    QObject *settingsViewQmlObject;

    QList<QMetaObject::Connection> accountConnections_;
    QString registeredName_;
    lrc::api::account::ConfProperties_t confProps_;
    SettingsMenu selectedMenu_ = SettingsMenu::General;
    lrc::api::profile::Type pastAccount_ = lrc::api::profile::Type::INVALID;
    QList<QPair<QString, float>> formatIndexList_;
};
