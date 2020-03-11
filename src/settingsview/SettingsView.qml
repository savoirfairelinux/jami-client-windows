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

    function connectCurrentAccount(){
        disconnectAccountConnections()
    }
    function disconnectAccountConnections(){}

    function setSelected(sel){
        if(selectedMenu === sel){return}
        switch(sel){
        case SettingsView.Account:
            avSettings.stopAudioMeter()
            avSettings.stopPreviewing()

            selectedMenu = sel
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

    property int minWidth: 768
    property int minHeight: 768
    property int textFontSize: 9

    property int selectedMenu: SettingsView.Account
    /*
     * signal to redirect the page to main view
     */
    signal settingsViewWindowNeedToShowMainViewWindow

    title: "Jami"
    visible: true
    width: 768
    height: 768
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: settingsViewRect
        anchors.fill: parent

        // TODO: hook up "isSIP" with LRC call
        property bool isSIP: false

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
                        avSettings.stopAudioMeter()
                        avSettings.stopPreviewing()
                        settingsViewWindowNeedToShowMainViewWindow()
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
