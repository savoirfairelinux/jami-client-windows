import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import net.jami.AccountModelAdapter 1.0

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
                    currentIndex: welcomePageStackId
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    property int welcomePageStackId: 0
                    property int createAccountPageId: 1
                    property int createSIPAccountPageId: 2
                    property int importFromBackupPageId: 3
                    property int backupKeysPageId: 4
                    property int importFromDevicePageId: 5
                    property int connectToAccountManagerPageId: 6
                    property int spinnerPageId: 7

                    WelcomePageLayout {
                        // welcome page, index 0
                        id: welcomePage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(false, true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(false, true);
                        }
                    }

                    CreateAccountPage {
                        // create account page, index 1
                        id: createAccountPage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(true);
                        }
                    }

                    CreateSIPAccountPage {
                        // create SIP account page, index 2
                        id: createSIPAccountPage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(true);
                        }
                    }

                    ImportFromBackupPage {
                        // import from backup page, index 3
                        id: importFromBackupPage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(true);
                        }
                    }

                    BackupKeyPage {
                        // backup keys page, index 4
                        id: backupKeysPage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(false);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(false);
                        }
                    }

                    ImportFromDevicePage {
                        // import from device page, index 5
                        id: importFromDevicePage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(true);
                        }
                    }

                    ConnectToAccountManagerPage {
                        // connect to account manager Page, index 6
                        id: connectToAccountManagerPage

                        onVisibleChanged: {
                            if(visible) navBarView.setNavBarVisibility(true);
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(true);
                        }
                    }

                    SpinnerPage {
                        // spinner Page, index 7
                        id: spinnerPage

                        onVisibleChanged: {
                            // hide all the buttons
                            if(visible) navBarView.setNavBarVisibility(false)
                        }

                        Component.onCompleted: {
                            navBarView.setNavBarVisibility(false)
                        }
                    }
                }
            }

            RowLayout {
                id: navBarView

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.fillWidth: true

                Layout.maximumHeight: 52
                Layout.preferredHeight: 52

                function setNavBarVisibility(navVisible, back=false) {
                    navBarView.visible = navVisible||back
                    btnNext.visible = navVisible
                    btnPevious.visible = navVisible
                    btnBack.visible = back && AccountModelAdapter.getAccountListSize()
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
                        if (controlPanelStackView.currentIndex == controlPanelStackView.createAccountPageId
                                || controlPanelStackView.currentIndex == controlPanelStackView.createSIPAccountPageId
                                || controlPanelStackView.currentIndex == controlPanelStackView.importFromBackupPageId
                                || controlPanelStackView.currentIndex == controlPanelStackView.importFromDevicePageId
                                || controlPanelStackView.currentIndex == controlPanelStackView.connectToAccountManagerPageId) {
                            controlPanelStackView.currentIndex = controlPanelStackView.welcomePageStackId
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
