import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Window {
    id: wizardViewWindow

    property int minWidth: 320
    property int minHeight: 400
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 768
    height: 873
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: wizardViewRect

        anchors.fill: parent

        ColumnLayout {
            id: contentLayout
            //anchors.fill: parent
            width: parent.width * 2 / 3
            height: parent.height * 2 / 3
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            anchors.centerIn: parent.center
            spacing: 40

            StackLayout {
                id: controlPanelStackView
                currentIndex: 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                // welcome page
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
                // create SIP account page
                ColumnLayout {
                    id: createSIPAccountPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                // import from backup page
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
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // backup keys page
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
                // import from device page
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
                        }
                    }

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                }
                // connect to account manager Page
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
                // spinner Page
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

                //Layout.preferredHeight: 52
                Layout.fillWidth: true
                Layout.fillHeight: false

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
                }
                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    id: btnBack
                    Layout.alignment: Qt.AlignHCenter
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Back")
                    font.pointSize: 10
                    font.kerning: true
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
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
                }
            }
        }
    }
}
