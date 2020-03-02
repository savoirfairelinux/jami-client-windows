import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    property alias text_pinFromDeviceAlias: pinFromDevice.text
    property alias text_passwordFromDeviceAlias: passwordFromDevice.text

    function initializeOnShowUp() {
        clearAllTextFields()
    }

    function clearAllTextFields() {
        pinFromDevice.clear()
        passwordFromDevice.clear()
    }

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

            selectByMouse: true
            placeholderText: qsTr("PIN")
        }

        TextField {
            id: passwordFromDevice

            Layout.alignment: Qt.AlignHCenter
            Layout.minimumWidth: 256
            Layout.minimumHeight: 30

            selectByMouse: true
            echoMode: TextInput.PasswordEchoOnEdit
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
