/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
#include <QScrollArea>
#include <QMovie>

#include "navwidget.h"
#include "lrcinstance.h"

#include "advancedsettingswidget.h"
#include "bannedlistmodel.h"

#include "linkdevwidget.h"
#include "ui_linkdevwidget.h"

 // general settings
#include "api/datatransfermodel.h"
#include "typedefs.h"

// av settings
#include "video/devicemodel.h"


namespace Ui {
    class SettingsWidget;
}

class SettingsWidget : public NavWidget
{
    Q_OBJECT
    SettingsWidget(const SettingsWidget& cpy);

public:
    explicit SettingsWidget(QWidget* parent = nullptr);
    ~SettingsWidget();
    void resize(int size);

    // NavWidget
    virtual void navigated(bool to);

public slots:
    void updateSettings(int size);

private:
    enum Button {accountSettingsButton, generalSettingsButton, avSettingsButton};
    enum RegName {BLANK, INVALIDFORM, TAKEN, FREE, SEARCHING};
    enum List {DevList, BannedContacts};

    void setSelected(Button sel);
    void updateAccountInfoDisplayed();

    void resizeEvent(QResizeEvent* event);

    Ui::SettingsWidget* ui;

    // *************************  Account Settings  *************************
    void passwordClicked();
    void avatarClicked();

    void afterNameLookup(lrc::api::account::LookupStatus status, const std::string& regName);
    QString registeredName_;

    bool validateRegNameForm(const QString& regName);

    AdvancedSettingsWidget* advancedSettingsWidget_;
    QScrollArea* scrollArea_;
    Button pastButton_ = Button::generalSettingsButton;
    bool advancedSettingsDropped_ = false;
    bool bannedContactsShown_ = false;

    int avatarSize_;

    void setRegNameUi(RegName stat);
    bool regNameBtn_ = false;

    const int itemHeight_ = 55;

    void removeDeviceSlot(int index);
    void unban(int index);

    void setConnections();
    QMovie* gif;

    LinkDevWidget* linkDevWidget;

private slots:
    void leaveSettingsSlot();
    void verifyRegisteredNameSlot();
    void beforeNameLookup();
    void receiveRegNameSlot(const std::string& accountID, lrc::api::account::LookupStatus status,
        const std::string& address, const std::string& name);
    void regNameRegisteredSlot();
    void setAccEnableSlot(int state);
    void delAccountSlot();

    void toggleAdvancedSettings();
    void toggleBannedContacts();
    void exportAccountSlot();

    void updateAndShowDevicesSlot();
    void updateAndShowBannedContactsSlot();

    void showLinkDevSlot();
    void showCurrentAccountSlot();

    void setButtonIconSlot(int frame);


    // *************************  General Settings  *************************
private:
    void populateGeneralSettings();
    QMetaObject::Connection accountAddedConnection_;

private slots:
    void setNotificationsSlot(int state);
    void checkForUpdateSlot();
    void setClosedOrMinSlot(int state);
    void openDownloadFolderSlot();
    void setAlwaysRecordingSlot(int state);
    void openRecordFolderSlot();
    void setUpdateIntervalSlot(int value);
    void setUpdateAutomaticSlot(int state);

    // *************************  Audio/Visual Settings  *************************
private:
    void populateAVSettings();
    void saveSizeIndex();
    Video::DeviceModel* deviceModel_;
    void showPreview();
    bool isLoading_;
    int currentResIndex = -1;

    void startVideo();
    void stopVideo();

private slots:
    void deviceIndexChanged(int index);
    void deviceBoxCurrentIndexChangedSlot(int index);
    void sizeBoxCurrentIndexChangedSlot(int index);
    void outputDevIndexChangedSlot(int index);
    void inputdevIndexChangedSlot(int index);

};
