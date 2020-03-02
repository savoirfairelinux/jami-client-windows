import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
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
