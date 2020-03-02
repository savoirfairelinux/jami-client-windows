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
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.LrcGeneralAdapter 1.0
import net.jami.PasswordDialogQMLObjectHolder 1.0

/*
 * PasswordDialog for changing password and exporting account
 */

Dialog {
    id: passwordDialog

    enum PasswordEnteringPurpose {
        ChangePassword,
        ExportAccount
    }
    readonly property int successCode: 200
    signal doneSignal(int code, int currentPurpose)

    property string path: ""
    property int purpose: PasswordDialog.ChangePassword

    onPurposeChanged: {
        if (purpose === PasswordDialog.ExportAccount) {
            passwordDialog.title = qsTr("Enter the password of this account")
        }
    }

    function haveDone(code, currentPurpose) {
        done(code)
        doneSignal(code, currentPurpose)
    }

    function validatePassword() {
        var acceptablePassword = confirmPasswordEdit.text
        btnChangePasswordConfirm.enabled = acceptablePassword

        if (acceptablePassword) {
            passwordEdit.background = passwordEdit.backgroundOptionList[1]
            confirmPasswordEdit.background = confirmPasswordEdit.backgroundOptionList[1]
            return
        }

        passwordEdit.background = passwordEdit.backgroundOptionList[2]
        confirmPasswordEdit.background = confirmPasswordEdit.backgroundOptionList[2]
    }

    function exportAccountQML() {
        var success = passwordDialogQMLObjectHolder.exportAccount(
                    currentPasswordEdit.text, path)

        spinnerLabel.visible = false
        if (success) {
            haveDone(successCode, purpose)
        } else {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }
    function savePasswordQML() {
        var success = AccountAdapter.savePassword(LrcGeneralAdapter.getCurrAccId(),currentPasswordEdit.text, passwordEdit.text)

        spinnerLabel.visible = false
        if (success) {
            AccountAdapter.setArchiveHasPassword(currentPasswordEdit.text.length !== 0)
            haveDone(successCode, purpose)
        } else {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }

    Component.onCompleted: {
        passwordDialogQMLObjectHolder.setPasswordDialogQmlObject(passwordDialog)
    }

    PasswordDialogQMLObjectHolder {
        id: passwordDialogQMLObjectHolder
    }

    implicitWidth: 440
    implicitHeight: 240

    ColumnLayout {
        anchors.fill: parent
        spacing: 7

        ColumnLayout {
            Layout.topMargin: 11
            Layout.leftMargin: 40
            Layout.rightMargin: 40
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true

            Layout.preferredWidth: 360
            Layout.maximumWidth: 360
            spacing: 7

            ColumnLayout {
                spacing: 0

                Layout.alignment: Qt.AlignHCenter

                Layout.preferredWidth: 356
                Layout.maximumWidth: 356

                TextField {
                    id: currentPasswordEdit

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30
                    Layout.fillWidth: true

                    echoMode: TextInput.PasswordEchoOnEdit
                    font.pointSize: 10
                    font.kerning: true
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    placeholderText: qsTr("Enter Current Password")

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.height / 2
                        border.color: "black"
                        border.width: 2
                    }

                    onTextChanged: {
                        if (purpose === PasswordDialog.ChangePassword) {
                            validatePassword()
                        }

                        if (currentPasswordEdit.text.length == 0) {
                            btnChangePasswordConfirm.enabled = false
                        } else {
                            wrongPasswordLabel.visible = false
                            btnChangePasswordConfirm.enabled = true
                        }
                    }
                }

                Label {
                    id: wrongPasswordLabel

                    Layout.minimumHeight: 12
                    Layout.preferredHeight: 12
                    Layout.maximumHeight: 12
                    Layout.fillWidth: true

                    font.pointSize: 8
                    font.kerning: true
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: qsTr("Current Password Incorrect")
                    color: "red"

                    visible: false
                }
            }

            Item {
                Layout.fillWidth: true

                Layout.minimumHeight: 8
                Layout.preferredHeight: 8
                Layout.maximumHeight: 8
            }

            TextField {
                id: passwordEdit

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30
                Layout.fillWidth: true

                visible: purpose === PasswordDialog.ChangePassword
                echoMode: TextInput.PasswordEchoOnEdit
                font.pointSize: 10
                font.kerning: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                placeholderText: qsTr("Enter New Password")

                background: backgroundOptionList[0]

                property list<Rectangle> backgroundOptionList: [
                    Rectangle {
                        anchors.fill: parent
                        radius: passwordEdit.height / 2
                        border.color: "black"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: passwordEdit.height / 2
                        border.color: "green"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: passwordEdit.height / 2
                        border.color: "red"
                        border.width: 2
                    }
                ]

                onTextChanged: {
                    validatePassword()
                }
            }

            Item {
                Layout.fillWidth: true

                Layout.minimumHeight: 8
                Layout.preferredHeight: 8
                Layout.maximumHeight: 8
            }

            TextField {
                id: confirmPasswordEdit

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30
                Layout.fillWidth: true

                visible: purpose === PasswordDialog.ChangePassword
                echoMode: TextInput.PasswordEchoOnEdit
                font.pointSize: 10
                font.kerning: true
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                placeholderText: qsTr("Confirm New Password")

                background: backgroundOptionList[0]

                property list<Rectangle> backgroundOptionList: [
                    Rectangle {
                        anchors.fill: parent
                        radius: confirmPasswordEdit.height / 2
                        border.color: "black"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: confirmPasswordEdit.height / 2
                        border.color: "green"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: confirmPasswordEdit.height / 2
                        border.color: "red"
                        border.width: 2
                    }
                ]

                onTextChanged: {
                    validatePassword()
                }
            }
        }

        Label {
            id: spinnerLabel

            visible: false

            Layout.minimumHeight: 20
            Layout.preferredHeight: 20
            Layout.maximumHeight: 20

            Layout.fillWidth: true

            background: Rectangle {
                anchors.fill: parent
                AnimatedImage {
                    Layout.alignment: Qt.AlignCenter

                    Layout.minimumHeight: 20
                    Layout.preferredHeight: 20
                    Layout.maximumHeight: 20

                    Layout.minimumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.maximumWidth: 20

                    source: "qrc:/images/ajax-loader.gif"

                    playing: spinnerLabel.visible
                    paused: false
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                }
            }
        }

        RowLayout {
            spacing: 7

            Layout.bottomMargin: 11
            Layout.fillWidth: true

            HoverableButton {
                id: btnChangePasswordConfirm

                Layout.maximumWidth: 130
                Layout.preferredWidth: 130
                Layout.minimumWidth: 130

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                text: qsTr("Confirm")
                font.pointSize: 10
                font.kerning: true

                radius: height / 2

                enabled: false

                onClicked: {
                    spinnerLabel.visible = true
                    if (purpose === PasswordDialog.ChangePassword) {
                        savePasswordQML()
                    } else {
                        exportAccountQML()
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            HoverableButton {
                id: btnChangePasswordCancel

                Layout.maximumWidth: 130
                Layout.preferredWidth: 130
                Layout.minimumWidth: 130

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                text: qsTr("Cancel")
                font.pointSize: 10
                font.kerning: true

                radius: height / 2

                onClicked: {
                    passwordDialog.reject()
                }
            }
        }
    }
}
