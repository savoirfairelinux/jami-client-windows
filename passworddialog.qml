import QtQuick 2.0
import QtQuick.Layouts 1.13
import QtQuick.Controls 2.13
import QtQuick 2.13
import QtQuick.Controls.Styles 1.4
import com.widget.passworddialogqml 1.0

Item {
    function getOldPasswords() {}

    function validatePassword() {
        var acceptablePassword = passwordEdit.text == confirmPasswordEdit.text
    }

    Rectangle {
        color: "green"
        ColumnLayout {
            minimumWidth: 440
            minimumHeight: 240
            ColumnLayout {
                // sections for the password line edit
                spacing: 7
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                ColumnLayout {
                    spacing: 0
                    margins: 0
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
                    onTextChanged: {

                    }
                }
                TextField {
                    id: confirmPasswordEdit
                    objectName: confirmPasswordEdit
                    placeholderText: qsTr("Confirm New Password")
                    background: Rectangle {
                        color: parent.color
                        border: 2
                        border.color: black
                    }
                    onTextChanged: {

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
                    }
                }
            }
        }
    }
}
