import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.PasswordDialogQMLObjectHolder 1.0

Dialog {
    id: passwordDialog

    enum PasswordEnteringPurpose {
        ChangePassword,
        ExportAccount
    }
    readonly property int successCode: 200
    signal successSignal(bool success)

    property string path: ""
    property int purpose: PasswordDialog.ChangePassword

    onPurposeChanged: {
        if (purpose === PasswordDialog.ExportAccount) {
            setTitle(qsTr("Enter the password of this account"))
        }
    }

    onVisibleChanged: {
        if (!visible) {
            successSignal(result === successCode)
        }
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
            done(successCode)
        } else {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }
    function savePasswordQML() {
        var success = savePassword(currentPasswordEdit.text, passwordEdit.text)

        spinnerLabel.visible = false
        if (success) {
            setArchiveHasPassword(currentPasswordEdit.text.length !== 0)
            done(successCode)
        } else {
            currentPasswordEdit.clear()
            btnChangePasswordConfirm.enabled = false
            wrongPasswordLabel.visible = true
        }
    }

    Component.onCompleted: {
        passwordDialogQMLObjectHolder.setPasswordDialogQmlObject(passwordDialog)
    }

    implicitWidth: 440
    implicitHeight: 240

    PasswordDialogQMLObjectHolder {
        id: passwordDialogQMLObjectHolder

        onIsExportingPasswordSuccess: {
            if (success) {
                passwordDialog.done(passwordDialog.successCode)
            } else {
                btnChangePasswordConfirm.enabled = false
            }
        }

        function validatePassword() {
            var acceptablePassword = (passwordEdit.text == confirmPasswordEdit.text)
            btnChangePasswordConfirm.enabled = acceptablePassword

            if (acceptablePassword) {
                passwordEdit.background = passwordEdit.backgroundOptionList[1]
                confirmPasswordEdit.background = confirmPasswordEdit.backgroundOptionList[1]
                return
            }

            passwordEdit.background = passwordEdit.backgroundOptionList[2]
            confirmPasswordEdit.background = confirmPasswordEdit.backgroundOptionList[2]
        }
    }

    Connections {
        id: exportAccountConnections
        target: passwordDialog

        enabled: purpose === PasswordDialog.ExportAccount ? true : false

        btnChangePasswordConfirm.onClicked: {
            spinnerLabel.visible = true

            // export account
        }
    }

    Connections {
        id: changePasswordConnections
        target: passwordDialog

        enabled: purpose === PasswordDialog.ChangePassword ? true : false

        currentPasswordEdit.onTextChanged: {
            validatePassword()
        }

        passwordEdit.onTextChanged: {
            validatePassword()
        }

        confirmPasswordEdit.onTextChanged: {
            validatePassword()
        }

        btnChangePasswordConfirm.onClicked: {
            spinnerLabel.visible = true
            savePasswordQML()
        }
    }

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
                        border.color: "transparent"
                        border.width: 2
                    }

                    onTextChanged: {
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
                        radius: parent.height / 2
                        border.color: "transparent"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.height / 2
                        border.color: "green"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.height / 2
                        border.color: "red"
                        border.width: 2
                    }
                ]
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
                        radius: parent.height / 2
                        border.color: "transparent"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.height / 2
                        border.color: "green"
                        border.width: 2
                    },
                    Rectangle {
                        anchors.fill: parent
                        radius: parent.height / 2
                        border.color: "red"
                        border.width: 2
                    }
                ]
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
