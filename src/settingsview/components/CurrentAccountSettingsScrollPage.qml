import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

import "../../commoncomponents"

Rectangle {
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
            Layout.maximumHeight: 30
            Layout.minimumHeight: 0
            Layout.preferredHeight: accountPageTitle.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            Label {
                id: accountPageTitle

                Layout.preferredWidth: 117

                Layout.maximumHeight: 25
                Layout.preferredHeight: 25
                Layout.minimumHeight: 25

                text: qsTr("Jami Account")

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
            id: scrowArea

            Layout.fillHeight: true
            Layout.fillWidth: true

            font.pointSize: 8
            font.kerning: true

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: 622

                implicitWidth: width
                implicitHeight: height

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.leftMargin: 30

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 24
                        Layout.preferredWidth: 24
                        Layout.minimumWidth: 24
                    }

                    ToggleSwitch {
                        id: accountEnableToggle

                        labelText: qsTr("Enable")
                        fontPointSize: 11
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

                            Rectangle {
                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261

                                Layout.minimumHeight: 261
                                Layout.preferredHeight: 261
                                Layout.maximumHeight: 261

                                Layout.leftMargin: 20

                                color: "blue"
                            }

                            TextField {
                                id: displayNameLineEdit

                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }
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

                            text: qsTr("Identity")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 10
                            Layout.preferredWidth: 10
                            Layout.minimumWidth: 10
                        }

                        ColumnLayout {
                            spacing: 7

                            Layout.fillWidth: true

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                Label {
                                    Layout.maximumWidth: 13
                                    Layout.preferredWidth: 13
                                    Layout.minimumWidth: 13

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Id")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                TextField {
                                    Layout.fillWidth: true

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 32

                                Layout.leftMargin: 20

                                layoutDirection: Qt.LeftToRight

                                Label {
                                    id: lblRegisteredName

                                    Layout.maximumWidth: 127
                                    Layout.preferredWidth: 127
                                    Layout.minimumWidth: 127

                                    Layout.minimumHeight: 32
                                    Layout.preferredHeight: 32
                                    Layout.maximumHeight: 32

                                    text: qsTr("Registered name")

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                RowLayout {
                                    spacing: 6
                                    Layout.fillWidth: true
                                    Layout.maximumHeight: 30
                                    Layout.alignment: Qt.AlignVCenter

                                    TextField {
                                        id: currentRegisteredID

                                        Layout.maximumWidth: 300
                                        Layout.preferredWidth: 300
                                        Layout.minimumWidth: 300

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30

                                        placeholderText: qsTr("Type here to register a username")

                                        font.pointSize: 10
                                        font.kerning: true

                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    Label {
                                        id: lookupStatusLabel

                                        Layout.maximumWidth: 30
                                        Layout.preferredWidth: 30
                                        Layout.minimumWidth: 30

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30
                                    }

                                    HoverableButton {
                                        id: btnRegisterName

                                        Layout.maximumWidth: 80
                                        Layout.preferredWidth: 80
                                        Layout.minimumWidth: 80

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30

                                        text: qsTr("Register")
                                        font.pointSize: 10
                                        font.kerning: true

                                        radius: height / 2
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                    }
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButton {
                                    id: passwdPushButton

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Change Password")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButton {
                                    id: btnExportAccount

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Export Account")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButton {
                                    id: btnDeletAccount

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Delete Account")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2
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

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 27
                            Layout.preferredHeight: 27
                            Layout.minimumHeight: 27

                            text: qsTr("Linked Device")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 10
                            Layout.preferredWidth: 10
                            Layout.minimumWidth: 10
                        }

                        ColumnLayout {
                            spacing: 7

                            Layout.fillWidth: true

                            ListView {
                                id: settingsListView
                                layer.mipmap: false
                                clip: false
                                visible: true

                                Layout.leftMargin: 20

                                Layout.fillWidth: true

                                Layout.minimumHeight: 164
                                Layout.preferredHeight: 164
                                Layout.maximumHeight: 164
                            }

                            HoverableButton {
                                id: linkDevPushButton

                                Layout.fillWidth: true

                                Layout.maximumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.minimumHeight: 30

                                radius: height / 2

                                text: qsTr("+Link Another Device")
                                font.pointSize: 10
                                font.kerning: true
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
                    }

                    // banned list view
                    ColumnLayout {
                        id: bannedContaactSection

                        Layout.fillWidth: true
                        spacing: 6

                        RowLayout {
                            Layout.leftMargin: 9
                            Layout.rightMargin: 8
                            Layout.topMargin: 1

                            Layout.fillWidth: true
                            Layout.maximumHeight: 30

                            Label {
                                Layout.preferredWidth: 164
                                Layout.minimumWidth: 164

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Banned Contact")
                                font.pointSize: 13
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true

                                Layout.maximumWidth: 10
                                Layout.preferredWidth: 10
                                Layout.minimumWidth: 10
                            }

                            HoverableButton {
                                Layout.maximumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.minimumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2

                                icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                icon.height: 24
                                icon.width: 24
                            }

                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }

                        ColumnLayout {
                            spacing: 6

                            Layout.leftMargin: 9
                            Layout.rightMargin: 8
                            Layout.bottomMargin: 9
                            Item {
                                Layout.fillWidth: true

                                Layout.minimumHeight: 10
                                Layout.preferredHeight: 10
                                Layout.maximumHeight: 10
                            }

                            ListView {
                                id: bannedListWidget

                                Layout.leftMargin: 20
                                Layout.fillWidth: true
                                Layout.minimumHeight: 150
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
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
                            id: advancedAccountSettingsPButton

                            Layout.minimumWidth: 180

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: qsTr("Advanced Account Settings")
                            font.pointSize: 10
                            font.kerning: true
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }

                // instantiate advance setting page
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
