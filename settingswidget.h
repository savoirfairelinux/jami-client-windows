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

#include "advancedsettingswidget.h"
#include "advancedsipsettingwidget.h"
#include "bannedlistmodel.h"
#include "linkdevicedialog.h"
#include "photoboothwidget.h"

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

    // NavWidget
    virtual void navigated(bool to);
    virtual void updateCustomUI();
public slots:
    virtual void slotAccountOnBoarded();

private:
    Ui::SettingsWidget* ui;

    enum Button {
        accountSettingsButton,
        generalSettingsButton,
        mediaSettingsButton
    };
    enum RegName {
        BLANK,
        INVALIDFORM,
        TAKEN,
        FREE,
        SEARCHING
    };

    void setAvatar(PhotoboothWidget* avatarWidget);
    void setSelected(Button sel);
    void updateAccountInfoDisplayed();
    void resizeEvent(QResizeEvent* event);
    bool sipPasswordHidden_{false};
    void passwordClicked();
    void afterNameLookup(lrc::api::account::LookupStatus status, const std::string& regName);
    bool validateRegNameForm(const QString& regName);
    void setRegNameUi(RegName stat);
    void removeDeviceSlot(int index);
    void unban(int index);
    void setConnections();
    void populateGeneralSettings();
    void populateAVSettings();
    void setFormatListForDevice(const std::string& device);
    void showPreview();
    void startVideo();
    void stopVideo();
    void toggleVideoSettings(bool enabled);
    void toggleVideoPreview(bool enabled);

    QString registeredName_;
    lrc::api::account::ConfProperties_t confProps_;
    AdvancedSettingsWidget* advancedSettingsWidget_;
    QScrollArea* scrollArea_;
    Button pastButton_ = Button::generalSettingsButton;
    bool advancedSettingsDropped_ = false;
    QList<QPair<std::string, float>> formatIndexList_;
    std::string currentDisplayedVideoDevice_;
    AdvancedSIPSettingsWidget* advancedSIPSettingsWidget_;
    QScrollArea* scrollSIPArea_;
    bool advancedSIPSettingsDropped_ = false;
    int avatarSize_;
    int avatarSIPSize_;
    bool regNameBtn_ = false;
    const int itemHeight_ = 55;

    QMovie* lookupSpinnerMovie_;
    QPixmap statusSuccessPixmap_;
    QPixmap statusInvalidPixmap_;
    QPixmap statusErrorPixmap_;

private slots:
    void leaveSettingsSlot();
    void verifyRegisteredNameSlot();
    void beforeNameLookup();
    void receiveRegNameSlot(const std::string& accountID, lrc::api::account::LookupStatus status,
        const std::string& address, const std::string& name);
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
    void openDownloadFolderSlot();
    void slotSetAlwaysRecording(bool state);
    void openRecordFolderSlot();
    void slotSetUpdateAutomatic(bool state);
    void outputDevIndexChangedSlot(int index);
    void inputdevIndexChangedSlot(int index);
    void slotDeviceBoxCurrentIndexChanged(int index);
    void slotFormatBoxCurrentIndexChanged(int index);
};
