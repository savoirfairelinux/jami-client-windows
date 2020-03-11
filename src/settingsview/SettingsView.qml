import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "components"

Window {
    id: settingsViewWindow

    enum SettingsMenu{
        Account,
        General,
        Media
    }

    enum RegName {
        BLANK,
        INVALIDFORM,
        TAKEN,
        FREE,
        SEARCHING
    }

    onVisibleChanged: {
        if(visible){
            setSelected(selectedMenu,true)
        }
    }

    function setSelected(sel, recovery = false){
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
        target: LRCInstance

        function onAccountListChanged(){
            slotAccountListChanged()
            accountListChangedConnection.enabled = false
        }
    }

    // slots
    function leaveSettingsSlot(){
        avSettings.stopAudioMeter()
        avSettings.stopPreviewing()
        if(!settingsViewRect.isSIP){
            currentAccountSettingsScrollWidget.stopBooth()
        } else {
            currentSIPAccountSettingsScrollWidget.stopBooth()
        }
        settingsViewWindowNeedToShowMainViewWindow()
    }

    function slotAccountListChanged(){
        var accountList = avmodel.getAccountList()
        if(accountList.length === 0) {
            setSelected(SettingsView.Account)
        } else {
            currentAccountSettingsScrollWidget.disconnectAccountConnections()
        }
        var device = avmodel.getDefaultDevice()
        if(device === ""){
            avmodel.setCurrentVideoCaptureDevice(device)
        }
    }

    property AVModel avmodel: AccountAdapter.avModel()
    property int profileType: SettingsAdaptor.getCurrentAccount_Profile_Info_Type()

    property int selectedMenu: SettingsView.Account
    /*
     * signal to redirect the page to main view
     */
    signal settingsViewWindowNeedToShowMainViewWindow

    property int minWidth: 768
    property int minHeight: 768
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 768
    height: 768
    minimumWidth: minWidth
    minimumHeight: minHeight

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

        CT.SplitView {
            anchors.fill: parent
            orientation: Qt.Horizontal

            Rectangle {
                id: leftSettingsWidget

                Layout.minimumWidth: 380
                Layout.preferredWidth: 380
                Layout.maximumWidth: parent.width / 2
                Layout.fillHeight: true
                LeftPanelView {
                    id: leftPanelView

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

                Layout.fillWidth: true
                Layout.fillHeight: true

                // current account setting scroll page, index 0
                CurrentAccountSettingsScrollPage {
                    id: currentAccountSettingsScrollWidget
                }

                // current SIP account setting scroll page, index 1
                CurrentSIPAccountSettingScrollPage {
                    id: currentSIPAccountSettingsScrollWidget
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
