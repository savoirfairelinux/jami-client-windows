import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    property alias usernameManagerEditAlias: usernameManagerEdit
    property alias passwordManagerEditAlias: passwordManagerEdit
    property alias accountManagerEditAlias: accountManagerEdit

    function initializeOnShowUp(){
        usernameManagerEdit.clear()
        passwordManagerEdit.clear()
        accountManagerEdit.clear()
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
