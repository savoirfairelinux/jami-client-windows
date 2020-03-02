import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

// welcome page, index 0
ColumnLayout {
    id: welcomePage
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 6

    Item {
        // put a spacer to make the buttons closs to the middle
        Layout.minimumHeight: 10
        Layout.maximumHeight: 10
        Layout.preferredHeight: 10
        Layout.fillWidth: true
    }
    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Label {
            id: welcomeLabel
            Layout.maximumHeight: 40
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Welcome to")
            font.pointSize: 30
            font.kerning: true
        }
    }
    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Label {
            id: welcomeLogo
            Layout.alignment: Qt.AlignCenter
            Layout.minimumWidth: 100
            Layout.minimumHeight: 100
            Layout.maximumWidth: 16777215
            Layout.maximumHeight: 16777215
            Layout.preferredWidth: 300
            Layout.preferredHeight: 150
            color: "transparent"
            background: Image {
                id: logoIMG
                source: "qrc:/images/logo-jami-standard-coul.png"
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
        }
    }
    Item {
        // put a spacer to make the buttons closs to the middle
        Layout.preferredHeight: 57
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Button {
            id: btnCreateLocalAccount
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            text: qsTr("Create local account")
            font.pointSize: 10
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }

            onClicked: {
                controlPanelStackView.currentIndex = 1
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Button {
            id: btnImportFromDevice
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            text: qsTr("Import from device")
            font.pointSize: 10
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }

            onClicked: {
                controlPanelStackView.currentIndex = 5
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Button {
            id: btnImportFromBackup
            Layout.alignment: Qt.AlignCenter
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            text: qsTr("Import from backup")
            font.pointSize: 10
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }

            onClicked: {
                controlPanelStackView.currentIndex = 3
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Button {
            id: btnShowAdvanced
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Show Advanced")
            font.pointSize: 8
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.transparentColor
                radius: height / 2
            }

            onClicked: {
                btnCreateAccountManager.visible = !btnCreateAccountManager.visible
                btnAddNewSIPAccount.visible = !btnAddNewSIPAccount.visible
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        Button {
            id: btnCreateAccountManager
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Connect to account manager")
            visible: false
            font.pointSize: 10
            font.kerning: true

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }

            onClicked: {
                controlPanelStackView.currentIndex = 6
            }
        }
    }
    RowLayout {
        spacing: 6
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter
        RoundButton {
            id: btnAddNewSIPAccount
            Layout.preferredWidth: 256
            Layout.preferredHeight: 30
            Layout.alignment: Qt.AlignCenter
            text: qsTr("Add a new SIP account")
            visible: false
            font.pointSize: 10
            font.kerning: true
            radius: height / 2

            background: Rectangle {
                anchors.fill: parent

                color: JamiTheme.releaseColor
                radius: height / 2
            }
            onClicked: {
                controlPanelStackView.currentIndex = 2
            }
        }
    }
    Item {
        // put a spacer to make the buttons closs to the middle
        Layout.minimumHeight: 25
        Layout.maximumHeight: 25
        Layout.preferredHeight: 25
        Layout.fillWidth: true
    }
}
