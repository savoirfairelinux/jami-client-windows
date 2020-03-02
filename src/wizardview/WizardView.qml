import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import "../constant"
import "components"

Window {
    id: wizardViewWindow

    property int layoutWidth: 401
    property int layoutHeight: 625
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: layoutWidth
    height: layoutHeight

    Rectangle {
        id: wizardViewRect
        anchors.fill: parent

        ColumnLayout {
            id: content
            anchors.fill: parent
            Layout.alignment: Qt.AlignHCenter
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true

                StackLayout {
                    id: controlPanelStackView
                    currentIndex: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    WelcomePageLayout {
                        // welcome page, index 0
                        id: welcomePage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(false,true,false)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(false,true,false)
                        }
                    }

                    CreateAccountPage {
                        // create account page, index 1
                        id: createAccountPage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    CreateSIPAccountPage {
                        // create SIP account page, index 2
                        id: createSIPAccountPage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    ImportFromBackupPage {
                        // import from backup page, index 3
                        id: importFromBackupPage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    BackupKeyPage {
                        // backup keys page, index 4
                        id: backupKeysPage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    ImportFromDevicePage {
                        // import from device page, index 5
                        id: importFromDevicePage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    ConnectToAccountManagerPage {
                        // connect to account manager Page, index 6
                        id: connectToAccountManagerPage

                        onVisibleChanged: {
                            if(visible) navBarView.setBtnsVisibility(true,false,true)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(true,false,true)
                        }
                    }

                    SpinnerPage {
                        // spinner Page, index 7
                        id: spinnerPage

                        onVisibleChanged: {
                            // hide all the buttons
                            if(visible) navBarView.setBtnsVisibility(false,false,false)
                        }

                        Component.onCompleted: {
                            navBarView.setBtnsVisibility(false,false,false)
                        }
                    }
                }
            }

            RowLayout {
                id: navBarView

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.fillWidth: true

                Layout.maximumHeight: 40
                Layout.preferredHeight: 40

                function setBtnsVisibility(btnPeviousVisible, btnBackVisible, btnNextVisible) {
                    btnPevious.visible = btnPeviousVisible
                    btnBack.visible = btnBackVisible
                    btnNext.visible = btnNextVisible
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RoundButton {
                    id: btnPevious
                    Layout.alignment: Qt.AlignLeft
                    width: 85
                    height: 30
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
                    width: 85
                    height: 30
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
                    width: 85
                    height: 30
                    radius: height / 2

                    Layout.minimumWidth: 85
                    Layout.preferredWidth: 85
                    Layout.maximumWidth: 85

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

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
