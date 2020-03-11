import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "../../commoncomponents"

Rectangle {
    property alias advaceSIPSettingVisible: advanceSIPSettingsView.visible

    function updateAccountInfoDisplayed(isTosetAvatar = true){
        displaySIPNameLineEdit.text = accountInfo_ProfileInfo_Alias
        usernameSIP.text = accountConfig_Username
        hostnameSIP.text = accountConfig_Hostname
        passSIPlineEdit.text = accountConfig_Password
        proxySIP.text = accountConfig_ProxyServer

        accountSIPEnableCheckBox.checked = accountInfo_Enabled

        if(isTosetAvatar) setAvatar()

        if(advanceSIPSettingsView.visible){
            advanceSIPSettingsView.updateAccountInfoDisplayedAdvanceSIP()
        }
    }

    property NewAccountModel accountModel:AccountAdapter.accoundModel()

    property bool accountInfo_Enabled: SettingsAdaptor.get_CurrentAccountInfo_Enabled()
    property string accountInfo_ProfileInfo_Alias: SettingsAdaptor.getCurrentAccount_Profile_Info_Alias()

    property string accountConfig_Username: SettingsAdaptor.getAccountConfig_Username()
    property string accountConfig_Hostname: SettingsAdaptor.getAccountConfig_Hostname()
    property string accountConfig_Password: SettingsAdaptor.getAccountConfig_Password()
    property string accountConfig_ProxyServer: SettingsAdaptor.getAccountConfig_ProxyServer()

    function isPhotoBoothOpened(){
        return currentSIPAccountAvatar.takePhotoState
    }

    function setAvatar(){
        currentSIPAccountAvatar.setAvatarPixmap(SettingsAdaptor.getAvatarImage_Base64(currentSIPAccountAvatar.boothWidht),SettingsAdaptor.getIsDefaultAvatar())
    }

    function stopBooth(){
        currentSIPAccountAvatar.stopBooth()
    }

    // slots
    function setAccEnableSlot(state){
        accountModel.setAccountEnabled(UtilsAdapter.getCurrAccId(),state)
    }

    function delAccountSlot(){}

    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 10
            Layout.minimumHeight: 10
            Layout.preferredHeight: 10

            Layout.alignment: Qt.AlignTop
        }

        RowLayout {
            spacing: 6

            Layout.alignment: Qt.AlignTop

            Layout.fillWidth: true
            Layout.maximumHeight: 31
            Layout.minimumHeight: 0
            Layout.preferredHeight: accountPageTitleSIP.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 48
                Layout.preferredWidth: 48
                Layout.minimumWidth: 48
            }

            Label {
                id: accountPageTitleSIP

                Layout.preferredWidth: 133

                Layout.preferredHeight: 31
                Layout.minimumHeight: 25

                text: qsTr("SIP Account")

                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            font.pointSize: 8
            font.kerning: true
            clip: true
            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: 598

                Item {
                    Layout.fillHeight: true

                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.leftMargin: 48

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 24
                        Layout.preferredWidth: 24
                        Layout.minimumWidth: 24
                    }

                    ToggleSwitch {
                        id: accountSIPEnableCheckBox

                        labelText: qsTr("Enable")
                        fontPointSize: 10

                        onCheckedChanged: {
                            setAccEnableSlot(checked)
                        }
                    }

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.minimumWidth: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 21
                            Layout.preferredHeight: 21
                            Layout.minimumHeight: 21

                            text: qsTr("Profile")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 10
                            Layout.preferredHeight: 10
                            Layout.minimumHeight: 10
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            layoutDirection: Qt.LeftToRight

                            spacing: 6

                            PhotoboothView{
                                id: currentSIPAccountAvatar

                                Layout.alignment: Qt.AlignHCenter

                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261
                                Layout.maximumHeight: 261
                                Layout.preferredHeight: 261
                                Layout.minimumHeight: 261

                                Layout.leftMargin: 20

                                onImageAcquired: {
                                    SettingsAdaptor.setCurrAccAvatar(imgBase64)
                                }

                                onImageCleared: {
                                    SettingsAdaptor.clearCurrentAvatar()
                                    setAvatar()
                                }
                            }

                            InfoLineEdit {
                                id: displaySIPNameLineEdit

                                fieldLayoutWidth: 261

                                Layout.leftMargin: 20

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter

                                onEditingFinished: {
                                    AccountAdapter.setCurrAccDisplayName(displaySIPNameLineEdit.text)
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.maximumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.minimumHeight: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 6

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 27
                            Layout.preferredHeight: 27
                            Layout.minimumHeight: 27

                            text: qsTr("Identity")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 10
                            Layout.preferredHeight: 10
                            Layout.minimumHeight: 10
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 6

                            GridLayout {
                                rows: 4
                                columns: 2
                                flow: GridLayout.LeftToRight
                                rowSpacing: 14
                                columnSpacing: 6

                                Layout.fillWidth: true

                                Layout.leftMargin: 20

                                // user name
                                Label {
                                    Layout.maximumWidth: 76
                                    Layout.preferredWidth: 76
                                    Layout.minimumWidth: 76

                                    Layout.maximumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.minimumHeight: 30

                                    text: qsTr("Username")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                InfoLineEdit {
                                    id: usernameSIP

                                    fieldLayoutWidth: 300

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter

                                    onEditingFinished: {
                                        SettingsAdaptor.setAccountConfig_Username(usernameSIP.text)
                                    }
                                }

                                // host name
                                Label {
                                    Layout.maximumWidth: 76
                                    Layout.preferredWidth: 76
                                    Layout.minimumWidth: 76

                                    Layout.maximumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.minimumHeight: 30

                                    text: qsTr("Hostname")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                InfoLineEdit {
                                    id: hostnameSIP

                                    fieldLayoutWidth: 300

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter

                                    onEditingFinished: {
                                        SettingsAdaptor.setAccountConfig_Hostname(hostnameSIP.text)
                                    }
                                }

                                // proxy
                                Label {
                                    Layout.maximumWidth: 76
                                    Layout.preferredWidth: 76
                                    Layout.minimumWidth: 76

                                    Layout.maximumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.minimumHeight: 30

                                    text: qsTr("Proxy")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                InfoLineEdit {
                                    id: proxySIP

                                    fieldLayoutWidth: 300

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter

                                    onEditingFinished: {
                                        SettingsAdaptor.setAccountConfig_ProxyServer(proxySIP.text)
                                    }
                                }

                                // password
                                Label {
                                    Layout.maximumWidth: 76
                                    Layout.preferredWidth: 76
                                    Layout.minimumWidth: 76

                                    Layout.maximumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.minimumHeight: 30

                                    text: qsTr("Password")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }


                                InfoLineEdit {
                                    id: passSIPlineEdit

                                    fieldLayoutWidth: 300

                                    font.pointSize: 10
                                    font.kerning: true

                                    echoMode: TextInput.PasswordEchoOnEdit
                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter

                                    onEditingFinished: {
                                        SettingsAdaptor.setAccountConfig_Password(passSIPlineEdit.text)
                                    }
                                }
                            }

                            Item {
                                Layout.fillWidth: true

                                Layout.maximumHeight: 10
                                Layout.preferredHeight: 10
                                Layout.minimumHeight: 10
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30
                                Layout.leftMargin: 20

                                HoverableButtonTextItem {
                                    id: btnSIPDeletAccount

                                    backgroundColor: "red"
                                    onEnterColor:Qt.rgba(150 / 256,0,0,0.7)
                                    onDisabledBackgroundColor: Qt.rgba(255 / 256,0,0,0.8)
                                    onPressColor: backgroundColor
                                    textColor:"white"

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.maximumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.minimumHeight: 30

                                    radius: height / 2

                                    text: qsTr("Delete Account")
                                    font.pointSize: 10
                                    font.kerning: true

                                    onClicked: {
                                        delAccountSlot()
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.maximumHeight: 40
                        Layout.preferredHeight: 40
                        Layout.minimumHeight: 40
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 30
                        Layout.preferredHeight: 30
                        Layout.maximumHeight: 30

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        HoverableButton {
                            id: advancedAccountSettingsSIPButton

                            Layout.minimumWidth: 180

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: qsTr("Advanced Account Settings")
                            font.pointSize: 10
                            font.kerning: true

                            icon.source: {
                                if (advanceSIPSettingsView.visible) {
                                    return "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                                } else {
                                    return "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                }
                            }

                            icon.height: 24
                            icon.width: 24

                            onClicked: {
                                advanceSIPSettingsView.visible = !advanceSIPSettingsView.visible
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 48
                    Layout.preferredHeight: 48
                    Layout.maximumHeight: 48
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.leftMargin: 30

                    // instantiate advance setting page
                    AdvancedSIPSettingsView {
                        id: advanceSIPSettingsView

                        Layout.leftMargin: 10
                        visible: false

                        onVisibleChanged: {
                            updateAccountInfoDisplayedAdvanceSIP()
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
