
/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
*                                                                         *
* This program is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU General Public License as published by    *
* the Free Software Foundation; either version 3 of the License, or       *
* (at your option) any later version.                                     *
*                                                                         *
* This program is distributed in the hope that it will be useful,         *
* but WITHOUT ANY WARRANTY; without even the implied warranty of          *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
* GNU General Public License for more details.                            *
*                                                                         *
* You should have received a copy of the GNU General Public License       *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
**************************************************************************/
import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.4
import com.widget.passworddialogqml 1.0

Rectangle {
    property int mode: 0

    function initialization(modeIndex) {
        mode = modeIndex
        btnChangePasswordConfirm.enabled = false
        wrongPasswordLabel.visible = false
        if (modeIndex == 0) {

            // mode is change password
        } else {
            passwordEdit.visible = false
            confirmPasswordEdit.visible = false
        }
    }

    function validatePassword() {
        var acceptablePassword = passwordEdit.text == confirmPasswordEdit.text
        if (acceptablePassword) {
            //set the boarder to correct style
            passwordEditBackground.border.color = "green"
            confirmPasswordEditBackground.border.color = "green"
        }
        // set the boarder to red
        passwordEditBackground.border.color = "red"
        confirmPasswordEditBackground.border.color = "red"

        btnChangePasswordConfirm.enabled = acceptablePassword
    }

    function savePassword(currentPasswordEditPassword, passwordEditPassword) {
        var success = passwordDialogQuickUiController.savePassword(
                    currentPasswordEditPassword, passwordEditPassword)
        if (!success) {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }

    function exportAccount(currentPasswordEditPassword) {
        var success = passwordDialogQuickUiController.exportAccount(
                    currentPasswordEditPassword)
        if (!success) {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }
    id: rootRec
    Layout.alignment: Qt.AlignCenter
    Layout.fillWidth: true
    Layout.fillHeight: true
    ColumnLayout {
        spacing: 7
        Layout.preferredWidth: rootRec.width
        Layout.preferredHeight: rootRec.height
        Layout.minimumHeight: rootRec.height
        Layout.maximumHeight: rootRec.height
        Layout.minimumWidth: rootRec.width
        Layout.maximumWidth: rootRec.width
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        ColumnLayout {
            // sections for the password line edit
            width: parent.width
            spacing: 7
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter
                Rectangle {
                    id: currentPasswordEditBackground
                    Layout.preferredWidth: rootRec.width
                    Layout.fillWidth: true
                    height: 30
                    border.width: 5
                    border.color: "black"
                    TextField {
                        id: currentPasswordEdit
                        width: parent.width
                        height: parent.height
                        Layout.fillWidth: true
                        anchors.fill: parent
                        placeholderText: qsTr("Enter Current Password")
                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        onTextChanged: {
                            if (mode == 0) {
                                if (!(confirmPasswordEdit.text == ""
                                      || passwordEdit.text == "")) {
                                    validatePassword()
                                }
                                if (currentPasswordEdit.text == "") {
                                    btnChangePasswordConfirm.enabled = false
                                } else {
                                    wrongPasswordLabel.visible = false
                                    btnChangePasswordConfirm.enabled = true
                                }
                            }
                        }
                    }
                }
                Label {
                    id: wrongPasswordLabel
                    Layout.preferredWidth: rootRec.width
                    objectName: "wrongPasswordLabel"
                    width: parent.width
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 12
                    anchors.bottom: parent.bottom
                    text: qsTr("Current Password Incorrect")
                }
            }
            Rectangle {
                id: passwordEditBackground
                Layout.preferredWidth: rootRec.width
                Layout.fillWidth: true
                height: 30
                border.width: 5
                border.color: "black"
                TextField {
                    id: passwordEdit
                    width: parent.width
                    height: parent.height
                    Layout.fillWidth: true
                    objectName: "passwordEdit"
                    placeholderText: qsTr("Enter New Password")
                    anchors.fill: parent
                    onTextChanged: {
                        if (mode == 0) {
                            if (!(confirmPasswordEdit.text == ""
                                  || passwordEdit.text == "")) {
                                validatePassword()
                            }
                        }
                    }
                }
            }
            Rectangle {
                id: confirmPasswordEditBackground
                Layout.preferredWidth: rootRec.width
                Layout.fillWidth: true
                height: 30
                border.width: 5
                border.color: "black"
                TextField {
                    id: confirmPasswordEdit
                    width: parent.width
                    height: parent.height
                    Layout.fillWidth: true
                    objectName: "confirmPasswordEdit"
                    placeholderText: qsTr("Confirm New Password")
                    anchors.fill: parent
                    onTextChanged: {
                        if (mode == 0) {
                            if (!(confirmPasswordEdit.text == ""
                                  || passwordEdit.text == "")) {
                                validatePassword()
                            }
                        }
                    }
                }
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.minimumHeight: 20
            Layout.minimumWidth: 20
            Layout.preferredWidth: rootRec.width
            AnimatedImage {
                id: spinnerLabel
                objectName: "spinnerLabel"
                anchors.horizontalCenter: parent.horizontalCenter
                source: "images/ajax-loader.gif"
            }
        }
        RowLayout {
            // section for the buttons
            height: 30
            Layout.maximumHeight: 30
            Layout.minimumHeight: 30
            Layout.preferredWidth: rootRec.width
            width: parent.width
            Layout.fillWidth: true
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            Button {
                id: btnChangePasswordConfirm
                objectName: "btnChangePasswordConfirm"
                text: qsTr("Confirm")
                anchors.left: parent.left
                height: parent.height
                Layout.fillHeight: true
                Layout.preferredWidth: 130
                width: 130
                onClicked: {
                    // what to do when comfirm button is clicked
                    if (mode == 0) {
                        savePassword()
                    } else {
                        exportAccount()
                    }
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            Button {
                id: btnChangePasswordCancel
                objectName: "btnChangePasswordCancel"
                text: qsTr("Cancel")
                anchors.right: parent.right
                height: parent.height
                Layout.fillHeight: true
                Layout.preferredWidth: 130
                width: 130
                onClicked: {
                    // what to do when cancel button is clicked
                    passwordDialogQuickUiController.cancelBtnClick()
                }
            }
        }
    }
}
