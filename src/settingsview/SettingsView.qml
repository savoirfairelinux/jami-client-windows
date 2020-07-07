/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
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

import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "components"

Rectangle {
    id: settingsViewWindow

    enum SettingsMenu{
        Account,
        General,
        Media
    }

    onVisibleChanged: {
        if(visible){
            setSelected(selectedMenu,true)
        }
    }

    function setSelected(sel, recovery = false){
        profileType = ClientWrapper.settingsAdaptor.getCurrentAccount_Profile_Info_Type()

        if(selectedMenu === sel && (!recovery)){return}
        switch(sel){
        case SettingsView.Account:
            currentAccountSettingsScrollWidget.connectCurrentAccount()

            avSettings.stopAudioMeter()
            avSettings.stopPreviewing()

            selectedMenu = sel

            if(!settingsViewRect.isSIP){
                if(currentAccountSettingsScrollWidget.isPhotoBoothOpened())
                {
                    currentAccountSettingsScrollWidget.setAvatar()
                }

                currentAccountSettingsScrollWidget.updateAccountInfoDisplayed()
            } else {
                if(currentSIPAccountSettingsScrollWidget.isPhotoBoothOpened()) {
                    currentSIPAccountSettingsScrollWidget.setAvatar()
                }
                currentSIPAccountSettingsScrollWidget.updateAccountInfoDisplayed()
            }
            break
        case SettingsView.General:
            try{
                avSettings.stopAudioMeter()
                avSettings.stopPreviewing()
            } catch(erro){}

            selectedMenu = sel
            generalSettings.populateGeneralSettings()
            break
        case SettingsView.Media:
            selectedMenu = sel

            avSettings.stopPreviewing()
            avSettings.populateAVSettings()
            avSettings.startAudioMeter()
            break
        }
    }

    Connections{
        id: accountListChangedConnection
        target: ClientWrapper.lrcInstance

        function onAccountListChanged(){
            slotAccountListChanged()
            accountListChangedConnection.enabled = false
        }
    }

    // slots
    function leaveSettingsSlot(showMainView = true){
        avSettings.stopAudioMeter()
        avSettings.stopPreviewing()
        if(!settingsViewRect.isSIP){
            currentAccountSettingsScrollWidget.stopBooth()
        } else {
            currentSIPAccountSettingsScrollWidget.stopBooth()
        }
        if (showMainView)
            settingsViewWindowNeedToShowMainViewWindow()
        else
            settingsViewWindowNeedToShowNewWizardWindow()
    }

    function slotAccountListChanged(){
        var accountList = ClientWrapper.accountModel.getAccountList()
        if(accountList.length === 0) {
            setSelected(SettingsView.Account)
        } else {
            currentAccountSettingsScrollWidget.disconnectAccountConnections()
        }
        var device = ClientWrapper.avmodel.getDefaultDevice()
        if(device.length === 0){
            ClientWrapper.avmodel.setCurrentVideoCaptureDevice(device)
        }
    }
    property int profileType: ClientWrapper.settingsAdaptor.getCurrentAccount_Profile_Info_Type()


    property int selectedMenu: SettingsView.Account
    /*
     * signal to redirect the page to main view
     */
    signal settingsViewWindowNeedToShowMainViewWindow
    signal settingsViewWindowNeedToShowNewWizardWindow

    property int textFontSize: 9

    visible: true

    Rectangle {
        id: settingsViewRect
        anchors.fill: parent

        property bool isSIP: {
            switch (profileType) {
            case Profile.Type.SIP:
                return true;
            default:
                return false;
            }
        }

        SplitView {
            anchors.fill: parent
            orientation: Qt.Horizontal

            handle: Rectangle {
                    implicitWidth: 3
                    implicitHeight: 3
                    color: JamiTheme.lightGrey_
                }

            Rectangle {
                id: leftSettingsWidget

                SplitView.minimumWidth: 200
                SplitView.preferredWidth: 200
                SplitView.maximumWidth: parent.width / 2
                SplitView.fillHeight: true
                LeftPanelView {
                    id: leftPanelView

                    contentViewportWidth: leftSettingsWidget.width
                    contentViewPortHeight: leftSettingsWidget.height

                    onBtnExitClicked:{
                        leaveSettingsSlot()
                    }

                    Connections {
                        target: leftPanelView.btnAccountSettings
                        function onCheckedToggledForRightPanel(checked) {
                            setSelected(SettingsView.Account)
                        }
                    }
                    Connections {
                        target: leftPanelView.btnGeneralSettings
                        function onCheckedToggledForRightPanel(checked) {
                            setSelected(SettingsView.General)
                        }
                    }
                    Connections {
                        target: leftPanelView.btnMediaSettings
                        function onCheckedToggledForRightPanel(checked) {
                            setSelected(SettingsView.Media)
                        }
                    }
                }
            }

            StackLayout {
                id: rightSettingsWidget

                property int pageIdCurrentAccountSettingsScrollPage: 0
                property int pageIdCurrentSIPAccountSettingScrollPage: 1
                property int pageIdGeneralSettingsPage: 2
                property int pageIdAvSettingPage: 3

                currentIndex: {
                    switch(selectedMenu){
                        case SettingsView.Account:
                            if(settingsViewRect.isSIP){
                                return pageIdCurrentSIPAccountSettingScrollPage
                            } else {
                                return pageIdCurrentAccountSettingsScrollPage
                            }
                        case SettingsView.General:
                            return pageIdGeneralSettingsPage
                        case SettingsView.Media:
                            return pageIdAvSettingPage
                    }
                }

                SplitView.fillWidth: true
                SplitView.fillHeight: true

                // current account setting scroll page, index 0
                CurrentAccountSettingsScrollPage {
                    id: currentAccountSettingsScrollWidget

                    onNavigateToMainView:{
                        leaveSettingsSlot()
                    }

                    onNavigateToNewWizardView: {
                        leaveSettingsSlot(false)
                    }
                }

                // current SIP account setting scroll page, index 1
                CurrentSIPAccountSettingScrollPage {
                    id: currentSIPAccountSettingsScrollWidget

                    onNavigateToMainView: {
                        leaveSettingsSlot()
                    }

                    onNavigateToNewWizardView: {
                        leaveSettingsSlot(false)
                    }
                }

                // general setting page, index 2
                GeneralSettingsPage {
                    id: generalSettings
                }

                // av setting page, index 3
                AvSettingPage {
                    id: avSettings
                }
            }
        }
    }
}