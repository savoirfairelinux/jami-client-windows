import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true

    property alias text_passwordFromBackupEditAlias: passwordFromBackupEdit.text

    function clearAllTextFields() {
        passwordFromBackupEdit.clear()
    }

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

                color: JamiTheme.releaseColor
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
