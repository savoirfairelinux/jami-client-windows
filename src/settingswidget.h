/***************************************************************************
 * Copyright (C) 2019-2019 by Savoir-faire Linux                           *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
 **************************************************************************/
#pragma once

#include <QMovie>
#include <QScrollArea>

#include "lrcinstance.h"
#include "navwidget.h"
#include "utils.h"

#include "advancedsettingswidget.h"
#include "advancedsipsettingwidget.h"
#include "bannedlistmodel.h"
#include "linkdevicedialog.h"
#include "photoboothwidget.h"
#include "previewwidget.h"

#include "api/datatransfermodel.h"
#include "typedefs.h"

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public NavWidget {
    Q_OBJECT
        SettingsWidget(const SettingsWidget& cpy);

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget();

    void setSelected(SettingsMenu sel);

    // NavWidget
    virtual void navigated(bool to);
    virtual void updateCustomUI();
public slots:
    virtual void slotAccountListChanged();

private:
    Ui::SettingsWidget* ui;

    enum RegName {
        BLANK,
        INVALIDFORM,
        TAKEN,
        FREE,
        SEARCHING
    };

    void setAvatar(PhotoboothWidget* avatarWidget);
    void updateAccountInfoDisplayed();
    void resizeEvent(QResizeEvent* event);
    bool sipPasswordHidden_ {false};
    void passwordClicked();
    void afterNameLookup(lrc::api::account::LookupStatus status, const QString& regName);
    bool validateRegNameForm(const QString& regName);
    void setRegNameUi(RegName stat);
    void removeDeviceSlot(int index);
    void unban(int index);
    void connectCurrentAccount();
    void disconnectAccountConnections();
    void populateGeneralSettings();
    void populateAVSettings();
    void populateVideoSettings();
    void setFormatListForDevice(const QString& device);
    void startPreviewing(bool force = false);
    void stopPreviewing();
    void startAudioMeter(bool async = true);
    void stopAudioMeter(bool async = true);
    void registerShortcuts();

    QList<QMetaObject::Connection> accountConnections_;
    QString registeredName_;
    lrc::api::account::ConfProperties_t confProps_;
    AdvancedSettingsWidget* advancedSettingsWidget_;
    QScrollArea* scrollArea_;
    SettingsMenu selectedMenu_ = SettingsMenu::General;
    lrc::api::profile::Type pastAccount_ = lrc::api::profile::Type::INVALID;
    bool advancedSettingsDropped_ = false;
    QList<QPair<QString, float>> formatIndexList_;
    AdvancedSIPSettingsWidget* advancedSIPSettingsWidget_;
    QScrollArea* scrollSIPArea_;
    bool advancedSIPSettingsDropped_ = false;
    int avatarSize_;
    int avatarSIPSize_;
    bool regNameBtn_ = false;
    const int itemHeight_ = 55;
    int previousDeviceSize_ {static_cast<int>(LRCInstance::avModel().getDevices().size())};
    bool deviceWasEmpty_ {false};
    QSet<int> keyPressed_;

    QMovie* lookupSpinnerMovie_;
    QPixmap statusSuccessPixmap_;
    QPixmap statusInvalidPixmap_;
    QPixmap statusErrorPixmap_;

private slots:
    void leaveSettingsSlot();
    void verifyRegisteredNameSlot();
    void beforeNameLookup();
    void receiveRegNameSlot(const QString& accountID, lrc::api::account::LookupStatus status,
                            const QString& address, const QString& name);
    void slotRegisterName();
    void setAccEnableSlot(int state);
    void delAccountSlot();
    void toggleAdvancedSIPSettings();
    void toggleAdvancedSettings();
    void toggleBannedContacts();
    void exportAccountSlot();
    void updateAndShowDevicesSlot();
    void updateAndShowBannedContactsSlot();
    void showLinkDevSlot();
    void slotSetNotifications(bool state);
    void checkForUpdateSlot();
    void slotSetClosedOrMin(bool state);
    void slotSetRunOnStartUp(bool state);
    void openDownloadFolderSlot();
    void openRecordFolderSlot();
    void slotAlwaysRecordingClicked(bool state);
    void slotRecordPreviewClicked(bool state);
    void slotRecordQualitySliderValueChanged(int value);
    void slotRecordQualitySliderSliderReleased();
    void slotSetUpdateAutomatic(bool state);
    void slotAudioOutputIndexChanged(int index);
    void slotAudioInputIndexChanged(int index);
    void slotDeviceBoxCurrentIndexChanged(int index);
    void slotFormatBoxCurrentIndexChanged(int index);
    void slotSetHardwareAccel(bool state);
    void slotVideoDeviceListChanged();
    void slotAudioMeter(const QString& id, float level);

};
