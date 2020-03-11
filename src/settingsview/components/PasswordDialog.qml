import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4

import "../../commoncomponents"

Dialog {
    id: passwordDialog

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
                        radius: 2
                        implicitWidth: 100
                        implicitHeight: 24
                        border.color: "#333"
                        border.width: 1
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
            }
        }

        Label {
            Layout.minimumHeight: 20
            Layout.preferredHeight: 20
            Layout.maximumHeight: 20
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
