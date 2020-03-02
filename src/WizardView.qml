import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Window {
    id: wizardViewWindow

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
        id: wizardViewRect

        anchors.fill: parent

        ColumnLayout {
            id: contentLayout
            width: parent.width * 2 / 3
            height: parent.height * 2 / 3
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            anchors.centerIn: parent.center
            spacing: 40

            StackLayout {
                id: controlPanelStackView
                currentIndex: 0
                Layout.fillWidth: true
                Layout.fillHeight: true

                // welcome page, index 0
                ColumnLayout {
                    id: welcomePage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 6

                    Item {
                        // put a spacer to make the buttons closs to the middle
                        Layout.minimumHeight: 10
                        Layout.maximumHeight: 10
                        Layout.preferredHeight: 10
                        Layout.fillWidth: true
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Label {
                            id: welcomeLabel
                            Layout.maximumHeight: 40
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Welcome to")
                            font.pointSize: 30
                            font.kerning: true
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Label {
                            id: welcomeLogo
                            Layout.alignment: Qt.AlignCenter
                            Layout.minimumWidth: 100
                            Layout.minimumHeight: 100
                            Layout.maximumWidth: 16777215
                            Layout.maximumHeight: 16777215
                            Layout.preferredWidth: 300
                            Layout.preferredHeight: 150
                            color: "transparent"
                            background: Image {
                                id: logoIMG
                                source: "qrc:/images/logo-jami-standard-coul.png"
                                fillMode: Image.PreserveAspectFit
                                mipmap: true
                            }
                        }
                    }
                    Item {
                        // put a spacer to make the buttons closs to the middle
                        Layout.preferredHeight: 57
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnCreateLocalAccount
                            Layout.alignment: Qt.AlignCenter
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            text: qsTr("Create local account")
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }

                            onClicked: {
                                controlPanelStackView.currentIndex = 1
                            }
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnImportFromDevice
                            Layout.alignment: Qt.AlignCenter
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            text: qsTr("Import from device")
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }

                            onClicked: {
                                controlPanelStackView.currentIndex = 5
                            }
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnImportFromBackup
                            Layout.alignment: Qt.AlignCenter
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            text: qsTr("Import from backup")
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }

                            onClicked: {
                                controlPanelStackView.currentIndex = 3
                            }
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnShowAdvanced
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Show Advanced")
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.transparentColor
                                radius: height / 2
                            }

                            onClicked: {
                                btnCreateAccountManager.visible = !btnCreateAccountManager.visible
                                btnAddNewSIPAccount.visible = !btnAddNewSIPAccount.visible
                            }
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnCreateAccountManager
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Connect to account manager")
                            visible: false
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }

                            onClicked: {
                                controlPanelStackView.currentIndex = 6
                            }
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnAddNewSIPAccount
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Add a new SIP account")
                            visible: false
                            font.pointSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }

                            onClicked: {
                                controlPanelStackView.currentIndex = 2
                            }
                        }
                    }
                    Item {
                        // put a spacer to make the buttons closs to the middle
                        Layout.minimumHeight: 25
                        Layout.maximumHeight: 25
                        Layout.preferredHeight: 25
                        Layout.fillWidth: true
                    }
                }
                // create account page, index 1
                ColumnLayout {
                    id: createAccountPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    spacing: 6

                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Label {
                        id: profileSectionLabel

                        Layout.maximumWidth: 368
                        Layout.preferredWidth: 368
                        Layout.maximumHeight: 21
                        Layout.preferredHeight: 21

                        Layout.alignment: Qt.AlignHCenter

                        text: qsTr("Profile")
                        font.pointSize: 13
                        font.kerning: true

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    Rectangle {
                        id: setAvatarWidget
                        Layout.alignment: Qt.AlignHCenter
                        Layout.maximumWidth: 261
                        Layout.preferredWidth: 261
                        Layout.minimumWidth: 261
                        Layout.maximumHeight: 261
                        Layout.preferredHeight: 261
                        Layout.minimumHeight: 261

                        color: "green"
                    }

                    RowLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignHCenter
                        Layout.maximumHeight: 30

                        Item {
                            Layout.fillWidth: true
                            Layout.maximumHeight: 10
                        }

                        TextField {
                            id: fullNameEdit

                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            Layout.alignment: Qt.AlignCenter

                            placeholderText: qsTr("Profile name")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignHCenter

                        spacing: 5
                        Label {
                            id: accountSectionLabel
                            Layout.alignment: Qt.AlignHCenter

                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.minimumWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 30

                            text: qsTr("Account")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        CheckBox {
                            id: signUpCheckbox
                            checked: false

                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261

                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 25

                            Layout.alignment: Qt.AlignHCenter

                            text: qsTr("Register public username")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        TextField {
                            id: usernameEdit

                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261

                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 25

                            Layout.alignment: Qt.AlignHCenter

                            placeholderText: qsTr("Choose yuor username")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        Item {
                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.minimumWidth: 261

                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 30

                            Layout.alignment: Qt.AlignHCenter
                        }
                    }
                }
                // create SIP account page, index 2
                ColumnLayout {
                    id: createSIPAccountPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    spacing: 6

                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Label {
                        id: sipProfileSectionLabel

                        Layout.maximumWidth: 368
                        Layout.preferredWidth: 368
                        Layout.maximumHeight: 21
                        Layout.preferredHeight: 21

                        Layout.alignment: Qt.AlignHCenter

                        text: qsTr("Profile")
                        font.pointSize: 13
                        font.kerning: true

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    Rectangle {
                        id: setSIPAvatarWidget
                        Layout.alignment: Qt.AlignHCenter
                        Layout.maximumWidth: 261
                        Layout.preferredWidth: 261
                        Layout.minimumWidth: 261
                        Layout.maximumHeight: 261
                        Layout.preferredHeight: 261
                        Layout.minimumHeight: 261

                        color: "blue"
                    }

                    RowLayout {
                        spacing: 0
                        Layout.alignment: Qt.AlignHCenter
                        Layout.maximumHeight: 30

                        Item {
                            Layout.fillWidth: true
                            Layout.maximumHeight: 10
                        }

                        TextField {
                            id: sipFullNameEdit

                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            Layout.alignment: Qt.AlignCenter

                            placeholderText: qsTr("Profile name")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                    }

                    Label {
                        id: sipAccountSectionLabel
                        Layout.maximumWidth: 368
                        Layout.preferredWidth: 368
                        Layout.maximumHeight: 21
                        Layout.preferredHeight: 21

                        Layout.alignment: Qt.AlignHCenter

                        text: qsTr("SIP Account")
                        font.pointSize: 12
                        font.kerning: true

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 7

                        Item {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumHeight: 40
                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 40
                        }

                        TextField {
                            id: sipServernameEdit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            placeholderText: qsTr("Server")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        TextField {
                            id: sipProxyEdit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            placeholderText: qsTr("Proxy")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        TextField {
                            id: sipUsernameEdit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            placeholderText: qsTr("Username")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        TextField {
                            id: sipPasswordEdit
                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 261
                            Layout.preferredWidth: 261
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            placeholderText: qsTr("Password")
                            font.pointSize: 10
                            font.kerning: true
                        }
                    }
                }
                // import from backup page, index 3
                ColumnLayout {
                    id: importFromBackupPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignCenter
                        Layout.maximumWidth: 366

                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.maximumHeight: 24
                            spacing: 0

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Label {
                                id: importFromBackupLabel
                                Layout.minimumHeight: 24
                                Layout.minimumWidth: 234
                                text: qsTr("Import from backup")
                                font.pointSize: 13
                                font.kerning: true
                                horizontalAlignment: Qt.AlignLeft
                                verticalAlignment: Qt.AlignVCenter
                            }

                            Button {
                                id: backupInfoBtn
                                Layout.alignment: Qt.AlignVCenter
                                Layout.minimumWidth: 24
                                Layout.maximumWidth: 24
                                Layout.minimumHeight: 24
                                Layout.maximumHeight: 24

                                icon.source: "/images/icons/info-24px.svg"
                                icon.width: width
                                icon.height: height

                                onClicked: {
                                    backupInfoLabel.visible = !backupInfoLabel.visible
                                }
                            }
                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }

                        Button {
                            id: fileImportBtn

                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 256
                            Layout.preferredWidth: 256
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            text: qsTr("Archive(none)")
                            font.pixelSize: 10
                            font.kerning: true

                            background: Rectangle {
                                anchors.fill: parent

                                color: ColorConstant.releaseColor
                                radius: height / 2
                            }
                        }

                        TextField {
                            id: passwordFromBackupEdit

                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 256
                            Layout.preferredWidth: 256
                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30

                            placeholderText: qsTr("Password")
                        }

                        Label {
                            id: backupInfoLabel

                            Layout.alignment: Qt.AlignHCenter
                            Layout.maximumWidth: 366
                            Layout.preferredWidth: 366

                            text: qsTr("You can obtain an archive by clicking on \"Export account\" in the account settings. This will create a .gz file on your device.")
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            visible: false
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // backup keys page, index 4
                ColumnLayout {
                    id: backupKeysPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignCenter
                        Layout.maximumWidth: 366

                        spacing: 12

                        Label {
                            id: backupKeysLabel
                            Layout.alignment: Qt.AlignHCenter

                            Layout.maximumWidth: 366
                            Layout.maximumHeight: 21
                            Layout.preferredWidth: 366
                            Layout.preferredHeight: 21

                            text: qsTr("Backup your account")
                            font.pointSize: 13
                            font.kerning: true
                        }
                        Label {
                            id: backupInfoLabel1
                            Layout.maximumWidth: 366
                            Layout.maximumHeight: 57
                            Layout.preferredWidth: 366
                            Layout.preferredHeight: 57

                            text: qsTr("This account only exists on this device. If you lost your device or uninstall the application,your account will be deleted. You can backup your account now or later.")
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignJustify
                            verticalAlignment: Text.AlignVCenter
                        }
                        CheckBox {
                            id: neverShowAgainBox
                            checked: false

                            Layout.maximumWidth: 366
                            Layout.maximumHeight: 19
                            Layout.preferredWidth: 366
                            Layout.preferredHeight: 19

                            text: qsTr("Never show me this again")
                            font.pointSize: 8
                        }
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.maximumHeight: 20

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillHeight: true
                                Layout.maximumWidth: 40
                                Layout.minimumWidth: 10
                            }

                            Button {
                                id: exportBtn

                                Layout.alignment: Qt.AlignVCenter
                                Layout.minimumWidth: 85
                                Layout.maximumWidth: 85
                                Layout.minimumHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Export")
                                font.pointSize: 10
                                font.kerning: true

                                background: Rectangle {
                                    anchors.fill: parent

                                    color: ColorConstant.releaseColor
                                    radius: height / 2
                                }
                            }

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Button {
                                id: skipBtn

                                Layout.alignment: Qt.AlignVCenter
                                Layout.minimumWidth: 85
                                Layout.maximumWidth: 85
                                Layout.minimumHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Skip")
                                font.pointSize: 10
                                font.kerning: true

                                background: Rectangle {
                                    anchors.fill: parent

                                    color: ColorConstant.releaseColor
                                    radius: height / 2
                                }
                            }

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillHeight: true
                                Layout.maximumWidth: 40
                                Layout.minimumWidth: 10
                            }
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // import from device page, index 5
                ColumnLayout {
                    id: importFromDevicePage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignCenter
                        Layout.fillWidth: true

                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.maximumHeight: 24
                            spacing: 0

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }

                            Label {
                                id: importFromDeviceLabel
                                Layout.minimumHeight: 24
                                Layout.minimumWidth: 234
                                text: qsTr("Import from device")
                                font.pointSize: 13
                                font.kerning: true
                            }

                            Button {
                                id: pinInfoBtn
                                Layout.alignment: Qt.AlignVCenter
                                Layout.minimumWidth: 24
                                Layout.maximumWidth: 24
                                Layout.minimumHeight: 24
                                Layout.maximumHeight: 24

                                icon.source: "/images/icons/info-24px.svg"
                                icon.width: width
                                icon.height: height

                                onClicked: {
                                    pinInfoLabel.visible = !pinInfoLabel.visible
                                }
                            }
                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }
                        TextField {
                            id: pinFromDevice

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.minimumHeight: 30

                            placeholderText: qsTr("PIN")
                        }

                        TextField {
                            id: passwordFromDevice

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.minimumHeight: 30

                            placeholderText: qsTr("Password")
                        }

                        Label {
                            id: pinInfoLabel

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.maximumWidth: 256

                            text: qsTr("To obtain a PIN (valid for 10 minutes), you need to open the account settings on the other device and click on \"Link to another device\".")
                            wrapMode: Text.WordWrap
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter

                            visible: false
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // connect to account manager Page, index 6
                ColumnLayout {
                    id: connectToAccountManagerPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    ColumnLayout {
                        Layout.alignment: Qt.AlignCenter
                        Layout.fillWidth: true

                        spacing: 12

                        Label {
                            id: signInLabel

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.preferredHeight: 21
                            text: qsTr("Sign in")
                            font.pointSize: 13
                            font.kerning: true
                        }

                        TextField {
                            id: usernameManagerEdit

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.minimumHeight: 30

                            placeholderText: qsTr("Username")
                        }

                        TextField {
                            id: passwordManagerEdit

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.minimumHeight: 30

                            placeholderText: qsTr("Password")
                        }

                        TextField {
                            id: accountManagerEdit

                            Layout.alignment: Qt.AlignHCenter
                            Layout.minimumWidth: 256
                            Layout.minimumHeight: 30

                            placeholderText: qsTr("Account Manager")
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // spinner Page, index 7
                ColumnLayout {
                    id: spinnerPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 6

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    Label {
                        id: spinnerLabel
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumWidth: 200
                        Layout.minimumHeight: 200

                        Layout.maximumWidth: 16777215
                        Layout.maximumHeight: 16777215

                        background: AnimatedImage {
                            playing: true
                            paused: false
                            source: "qrc:/images/jami_eclipse_spinner.gif"
                            fillMode: Image.PreserveAspectFit
                            mipmap: true
                        }
                    }
                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    Label {
                        id: progressLabel
                        Layout.alignment: Qt.AlignHCenter
                        text: "Generating your Jami account"
                        font.pointSize: 11
                        font.kerning: true
                    }
                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumHeight: 20
                        Layout.maximumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                    }
                }
            }

            RowLayout {
                id: navBarView
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    id: btnPevious
                    Layout.alignment: Qt.AlignLeft
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Previous")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: height / 2
                        color: parent.hovered ? ColorConstant.hoverColor : ColorConstant.releaseColor
                    }

                    onClicked: {
                        // TODO: stop photobooth previewing
                        // Disconnect registered name found Connection
                        // deal with look up status label and collapsible password widget
                        // switch to welcome page
                        if (controlPanelStackView.currentIndex == 1
                                || controlPanelStackView.currentIndex == 2
                                || controlPanelStackView.currentIndex == 3
                                || controlPanelStackView.currentIndex == 5
                                || controlPanelStackView.currentIndex == 6) {
                            controlPanelStackView.currentIndex = 0
                        }
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                Button {
                    id: btnBack
                    Layout.alignment: Qt.AlignLeft
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Back")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: height / 2
                        color: parent.hovered ? ColorConstant.hoverColor : ColorConstant.releaseColor
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                Button {
                    id: btnNext
                    Layout.alignment: Qt.AlignRight
                    width: layout.preferredWidth
                    height: layout.preferredHeight

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Next")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: height / 2
                        color: parent.hovered ? ColorConstant.hoverColor : ColorConstant.releaseColor
                    }
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
