/***************************************************************************
* Copyright (C) 2019-2020 by Savoir-faire Linux                            *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                       *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/

import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.3

import "../../constant"

ColumnLayout {
    property alias text_passwordFromBackupEditAlias: passwordFromBackupEdit.text
    property string fileImportBtnText: qsTr("Archive(none)")

    signal importFromFile_Dialog_Accepted(string fileDir)

    function clearAllTextFields() {
        passwordFromBackupEdit.clear()
        fileImportBtnText = qsTr("Archive(none)")
    }

    FileDialog {
        id: importFromFile_Dialog

        selectExisting: true
        selectMultiple: false
        title: qsTr("Open File")
        folder: shortcuts.home + "/Desktop"

        nameFilters: [qsTr("Jami archive files") + " (*.gz)", qsTr(
                "All files") + " (*)"]

        onAccepted: {
            importFromFile_Dialog_Accepted(fileUrl)
        }
    }

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

            text: fileImportBtnText
            font.pixelSize: 10
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }

            onClicked: {
                importFromFile_Dialog.open()
            }
        }

        TextField {
            id: passwordFromBackupEdit

            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: 256
            Layout.preferredWidth: 256
            Layout.maximumHeight: 30
            Layout.preferredHeight: 30

            selectByMouse: true
            echoMode: TextInput.PasswordEchoOnEdit
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
