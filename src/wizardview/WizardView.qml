import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import "../constant"
import "components"

Window {
    id: wizardViewWindow

    property int minWidth: 768
    property int minHeight: 768
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 768
    height: 768
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: wizardViewRect

        anchors.fill: parent

        ColumnLayout {
            id: contentLayout
            width: parent.width * 2 / 3
            height: parent.height * 2 / 3
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            anchors.centerIn: parent.center
            spacing: 40

            StackLayout {
                id: controlPanelStackView
                currentIndex: 0
                Layout.fillWidth: true
                Layout.fillHeight: true

                WelcomePageLayout {
                    // welcome page, index 0
                    id: welcomePage
                }

                CreateAccountPage {
                    // create account page, index 1
                    id: createAccountPage
                }

                CreateSIPAccountPage {
                    // create SIP account page, index 2
                    id: createSIPAccountPage
                }

                ImportFromBackupPage {
                    // import from backup page, index 3
                    id: importFromBackupPage
                }

                BackupKeyPage {
                    // backup keys page, index 4
                    id: backupKeysPage
                }

                ImportFromDevicePage {
                    // import from device page, index 5
                    id: importFromDevicePage
                }

                ConnectToAccountManagerPage {
                    // connect to account manager Page, index 6
                    id: connectToAccountManagerPage
                }

                SpinnerPage {
                    // spinner Page, index 7
                    id: spinnerPage
                }
            }

            RowLayout {
                id: navBarView
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.fillWidth: true

                Layout.maximumWidth: 452
                Layout.preferredWidth: 452
                Layout.maximumHeight: 52
                Layout.preferredHeight: 52

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RoundButton {
                    id: btnPevious
                    Layout.alignment: Qt.AlignLeft
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    radius: height / 2

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Previous")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }

                    onClicked: {
                        // TODO: stop photobooth previewing
                        // Disconnect registered name found Connection
                        // deal with look up status label and collapsible password widget
                        // switch to welcome page
                        if (controlPanelStackView.currentIndex == 1
                                || controlPanelStackView.currentIndex == 2
                                || controlPanelStackView.currentIndex == 3
                                || controlPanelStackView.currentIndex == 5
                                || controlPanelStackView.currentIndex == 6) {
                            controlPanelStackView.currentIndex = 0
                        }
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                RoundButton {
                    id: btnBack
                    Layout.alignment: Qt.AlignLeft
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    radius: height / 2

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Back")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                RoundButton {
                    id: btnNext
                    Layout.alignment: Qt.AlignRight
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    radius: height / 2

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Next")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
