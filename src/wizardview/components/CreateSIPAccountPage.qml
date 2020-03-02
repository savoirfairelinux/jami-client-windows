/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Yang Wang <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    property alias text_sipFullNameEditAlias: sipFullNameEdit.text
    property alias text_sipServernameEditAlias: sipServernameEdit.text
    property alias text_sipProxyEditAlias: sipProxyEdit.text
    property alias text_sipUsernameEditAlias: sipUsernameEdit.text
    property alias text_sipPasswordEditAlias: sipPasswordEdit.text

    function initializeOnShowUp() {
        clearAllTextFields()
    }

    function clearAllTextFields() {
        sipUsernameEdit.clear()
        sipPasswordEdit.clear()
        sipServernameEdit.clear()
        sipProxyEdit.clear()
        sipFullNameEdit.clear()
        sipUsernameEdit.clear()
    }
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

            selectByMouse: true
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

            selectByMouse: true
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

            selectByMouse: true
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

            selectByMouse: true
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

            selectByMouse: true
            echoMode: TextInput.PasswordEchoOnEdit
            placeholderText: qsTr("Password")
            font.pointSize: 10
            font.kerning: true
        }
    }
}
