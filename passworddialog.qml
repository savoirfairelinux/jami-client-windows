import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.4
import com.widget.passworddialogqml 1.0

Item {
    property int mode: 0

    function initialization(modeIndex) {
        mode = modeIndex
        btnChangePasswordConfirm.enabled(false)
        wrongPasswordLabel.visible(false)
        if (modeIndex == 0) {

            // mode is change password
        } else {
            passwordEdit.visible(false)
            confirmPasswordEdit.visible(false)
        }
    }

    function validatePassword() {
        var acceptablePassword = passwordEdit.text == confirmPasswordEdit.text
        btnChangePasswordConfirm.enabled(acceptablePassword)
        if (acceptablePassword) {
            //set the boarder to correct style
            passwordEditBackground.color = "green"
            confirmPasswordEditBackground.color = "green"
        }
        // set the boarder to red
        passwordEditBackground.color = "red"
        confirmPasswordEditBackground.color = "red"
    }

    function savePassword(currentPasswordEditPassword, passwordEditPassword) {
        var success = passwordDialogQuickUiController.savePassword(
                    currentPasswordEditPassword, passwordEditPassword)
        if (!success) {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled(false)
            wrongPasswordLabel.visible(true)
        }
    }

    function exportAccount(currentPasswordEditPassword) {
        var success = passwordDialogQuickUiController.exportAccount(
                    currentPasswordEditPassword)
        if (!success) {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled(false)
            wrongPasswordLabel.visible(true)
        }
    }

    Rectangle {
        color: "green"
        ColumnLayout {
            ColumnLayout {
                // sections for the password line edit
                spacing: 7
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                ColumnLayout {
                    spacing: 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    TextField {
                        id: currentPasswordEdit
                        width: parent.width
                        placeholderText: qsTr("Enter Current Password")
                        height: 30

                        anchors.top: parent.top
                        anchors.horizontalCenter: parent.horizontalCenter
                        onTextChanged: {

                        }
                    }
                    Label {
                        id: wrongPasswordLabel
                        objectName: wrongPasswordLabel
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 30
                        anchors.bottom: parent.bottom
                        text: qsTr("Current Password Incorrect")
                    }
                }
                TextField {
                    id: passwordEdit
                    objectName: passwordEdit
                    placeholderText: qsTr("Enter New Password")
                    style: TextFieldStyle {
                        Rectangle {
                            id: passwordEditBackground
                            color: parent.color
                            border: 2
                            border.color: black
                        }
                    }
                    onTextChanged: {
                        validatePassword()
                    }
                }
                TextField {
                    id: confirmPasswordEdit
                    objectName: confirmPasswordEdit
                    placeholderText: qsTr("Confirm New Password")
                    style: TextFieldStyle {
                        Rectangle {
                            id: confirmPasswordEditBackground
                            color: parent.color
                            border: 2
                            border.color: black
                        }
                    }
                    onTextChanged: {
                        validatePassword()
                    }
                }
            }
            AnimatedImage {
                id: spinnerLabel
                objectName: spinnerLabel
                anchors.horizontalCenter: parent.horizontalCenter
                source: "images/ajax-loader.gif"
            }
            RowLayout {
                // section for the buttons
                height: 30
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                Button {
                    id: btnChangePasswordConfirm
                    text: qsTr("Confirm")
                    anchors.left: parent.left
                    height: parent.height
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

                Button {
                    id: btnChangePasswordCancel
                    objectName: btnChangePasswordCancel
                    text: qsTr("Cancel")
                    anchors.right: parent.right
                    height: parent.height
                    width: 130
                    onClicked: {
                        // what to do when cancel button is clicked
                        passwordDialogQuickUiController.cancelBtnClick()
                    }
                }
            }
        }
    }
}
