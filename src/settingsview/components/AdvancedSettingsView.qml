import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

import "../../commoncomponents"

ColumnLayout {
    function updateAccountInfoDisplayedAdvance(){}

    spacing: 6

    Layout.fillWidth: true

    Item {
        Layout.fillWidth: true

        Layout.minimumHeight: 24
        Layout.preferredHeight: 24
        Layout.maximumHeight: 24
    }

    ColumnLayout {
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Call Settings")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            ToggleSwitch {
                id: checkBoxUntrusted

                Layout.leftMargin: 20

                labelText: qsTr("Allow Calls From Unstructed Peers")
                fontPointSize: 10
            }

            ToggleSwitch {
                id: checkBoxAutoAnswer

                Layout.fillWidth: true
                Layout.leftMargin: 20

                labelText: qsTr("Auto Answer Calls")
                fontPointSize: 10
            }

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.maximumHeight: 30

                ToggleSwitch {
                    id: checkBoxCustomRingtone

                    labelText: qsTr("Enable Custom Ringtone")
                    fontPointSize: 10
                }

                HoverableButton {
                    id: btnRingtone

                    Layout.minimumWidth: 300
                    Layout.preferredWidth: 300
                    Layout.maximumWidth: 300

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    radius: height / 2

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
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
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Name Server")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.maximumHeight: 29

            Label {
                Layout.minimumWidth: 60

                Layout.minimumHeight: 29
                Layout.preferredHeight: 29
                Layout.maximumHeight: 29

                text: qsTr("Address")

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            InfoLineEdit {
                id: lineEditNameServer

                fieldLayoutWidth: 300
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
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
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("OpenDHT Configuration")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20

                ToggleSwitch {
                    id: checkBoxEnableProxy

                    labelText: qsTr("Enable proxy")
                    fontPointSize: 10
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                InfoLineEdit {
                    id: lineEditProxy

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20

                Label {
                    id: labelBootstrap

                    Layout.minimumWidth: 72
                    Layout.preferredWidth: 72

                    Layout.minimumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.maximumHeight: 29

                    text: qsTr("Bootstrap")
                    font.pointSize: 10
                    font.kerning: true
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                InfoLineEdit {
                    id: lineEditBootstrap

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    ColumnLayout {
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Security")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            GridLayout {
                rows: 4
                columns: 2
                rowSpacing: 0
                columnSpacing: 6

                Layout.fillWidth: true
                Layout.leftMargin: 20

                // CA Certificate
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("CA Certificate")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnCACert

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
                }

                // User Certificate
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("User Certificate")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnUserCert

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
                }

                // Private Key
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("Private Key")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnPrivateKey

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
                }

                // Private key password
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.maximumHeight: 29

                    text: qsTr("Private Key Password")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditCertPassword

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
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
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true
            Layout.topMargin: 10

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Connectivity")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            GridLayout {
                Layout.leftMargin: 20
                Layout.fillWidth: true

                rows: 6
                columns: 3
                rowSpacing: 6
                columnSpacing: 6

                // row 1
                ToggleSwitch {
                    id: checkBoxUPnP

                    Layout.row: 0
                    Layout.column: 0

                    labelText: qsTr("Use UPnP")
                    fontPointSize: 10
                }

                Item {
                    Layout.row: 0
                    Layout.column: 1

                    Layout.fillHeight: true

                    Layout.minimumWidth: 40
                    Layout.preferredWidth: 40
                    Layout.maximumWidth: 40
                }

                // row 2
                ToggleSwitch {
                    id: checkBoxTurn

                    Layout.row: 1
                    Layout.column: 0

                    labelText: qsTr("Use TURN")
                    fontPointSize: 10
                }

                // row 3
                Label {
                    Layout.row: 2
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Address")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnAddress

                    Layout.row: 2
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                //row 4
                Label {
                    Layout.row: 3
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Username")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnUsername

                    Layout.row: 3
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                //row 5
                Label {
                    Layout.row: 4
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Password")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnPassword
                    layer.mipmap: false

                    Layout.row: 4
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                // row 6
                ToggleSwitch {
                    id: checkBoxSTUNEnable

                    Layout.row: 5
                    Layout.column: 0

                    labelText: qsTr("Use STUN")
                    fontPointSize: 10
                }

                InfoLineEdit {
                    id: lineEditSTUNAddress

                    Layout.row: 5
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    placeholderText: qsTr("STUN Address")

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
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
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true
            Layout.topMargin: 10

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Media")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            ToggleSwitch {
                id: videoCheckBox

                Layout.leftMargin: 20

                labelText: qsTr("Enable Video")
                fontPointSize: 10
            }

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20

                ColumnLayout {
                    spacing: 6
                    //Layout.fillWidth: true
                    Layout.maximumWidth: 348

                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true

                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillWidth: true

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Video Codecs")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.minimumWidth: 20
                            Layout.preferredWidth: 20
                            Layout.maximumWidth: 20
                        }

                        HoverableButton {
                            id: videoDownPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2
                            scale: 1

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                            icon.width: 32
                            icon.height: 32
                        }

                        HoverableButton {
                            id: videoUpPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                            icon.width: 32
                            icon.height: 32
                        }
                    }

                    ListView {
                        id: videoListWidget

                        Layout.minimumWidth: 348
                        Layout.preferredWidth: 348
                        Layout.maximumWidth: 348

                        Layout.minimumHeight: 192
                        Layout.preferredHeight: 192
                        Layout.maximumHeight: 192
                    }
                }

                ColumnLayout {
                    spacing: 6
                    //Layout.fillWidth: true
                    Layout.maximumWidth: 348

                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true

                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillWidth: true

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Audio Codecs")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.minimumWidth: 20
                            Layout.preferredWidth: 20
                            Layout.maximumWidth: 20
                        }

                        HoverableButton {
                            id: audioDownPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                            icon.width: 32
                            icon.height: 32
                        }

                        HoverableButton {
                            id: audioUpPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                            icon.width: 32
                            icon.height: 32
                        }
                    }

                    ListView {
                        id: audioListWidget

                        Layout.minimumWidth: 348
                        Layout.preferredWidth: 348
                        Layout.maximumWidth: 348

                        Layout.minimumHeight: 192
                        Layout.preferredHeight: 192
                        Layout.maximumHeight: 192
                    }
                }
            }
        }
    }

    Item {
        Layout.fillWidth: true

        Layout.minimumHeight: 48
        Layout.preferredHeight: 48
        Layout.maximumHeight: 48
    }
}
