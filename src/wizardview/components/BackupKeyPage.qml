import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
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

                    color: JamiTheme.releaseColor
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

                    color: JamiTheme.releaseColor
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
