import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

Item {
    GridLayout {
        id: containerWidget
        rows: 1
        columns: 1
        flow: GridLayout.LeftToRight

        anchors.fill: parent
        Layout.fillHeight: true
        Layout.fillWidth: true
        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            ColumnLayout {
                id: controlPanelLayout
                Layout.alignment: Qt.AlignCenter

                Layout.maximumWidth: parent.width * (2 / 3)
                Layout.maximumWidth: parent.height * (2 / 3)
                Layout.minimumWidth: parent.width * (1 / 3)
                Layout.minimumHeight: parent.height * (1 / 3)
                Layout.preferredWidth: 512
                Layout.preferredHeight: 638

                topMargin: 0
                bottomMargin: 30
                leftMargin: 30
                rightMargin: 30

                spacing: 40

                RowLayout {
                    topMargin: 10
                    spacing: 6
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    StackLayout {
                        id: navStack
                        currentIndex: 0

                        // welcome page
                        ColumnLayout {
                            id: welcomePage
                            Layout.anchors.fill: true
                            margins: 9
                            spacing: 6
                            Item {
                                // put a spacer to make the buttons closs to the middle
                                Layout.minimumHeight: 57
                                Layout.maximumHeight: 57
                                Layout.preferredHeight: 57
                                Layout.fillWidth: true
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    id: welcomeLabel
                                    text: "Welcome to"
                                    font.pointSize: 30
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                Layout.fillWidth: true
                                alignment: Qt.AlignHCenter
                                Label {
                                    id: welcomeLogo
                                    Layout.minimumWidth: 0
                                    Layout.minimumHeight: 0
                                    Layout.maximumWidth: 16777215
                                    Layout.maximumHeight: 16777215
                                    background: Image {
                                        id: logoIMG
                                        source: "qrc:/images/logo-jami-standard-coul.png"
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
                                Button {
                                    id: btnCreateLocalAccount
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Create local account"
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Button {
                                    id: btnImportFromDevice
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Import from device"
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Button {
                                    id: btnImportFromBackup
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Import from backup"
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Button {
                                    id: btnShowAdvanced
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Show Advanced"
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Button {
                                    id: btnCreateAccountManager
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Connect to account manager"
                                    visible: false
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Button {
                                    id: btnAddNewSIPAccount
                                    Layout.preferredWidth: 256
                                    Layout.preferredHeight: 30
                                    text: "Add a new SIP account"
                                    visible: false
                                    font.pointSize: 10
                                    font.kerning: true
                                }
                            }
                            Item {
                                // put a spacer to make the buttons closs to the middle
                                Layout.minimumHeight: 65
                                Layout.maximumHeight: 65
                                Layout.preferredHeight: 65
                                Layout.fillWidth: true
                            }
                        }
                        // create account page

                        // create SIP account page

                        // import form backup page

                        // backup keys page

                        // import form device page

                        // connect to account manage page

                        // spinner page
                        ColumnLayout {
                            id: spinnerPage
                            StackLayout.count: 1

                            Layout.anchors.fill: true
                            margins: 9
                            spacing: 6
                            alignment: Qt.AlignHCenter

                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                            Label {
                                id: spinnerLabel
                                Layout.minimumWidth: 0
                                Layout.minimumHeight: 0

                                Layout.maximumWidth: 16777215
                                Layout.maximumHeight: 16777215

                                background: Image {
                                    source: "qrc:/images/jami_eclipse_spinner.gif"
                                }
                            }
                            Item {
                                Layout.preferredHeight: 40
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                            Label {
                                id: progressLabel
                                text: "Generating your Jami account"
                                font.pointSize: 11
                                font.kerning: true
                            }
                            Item {
                                Layout.minimumHeight: 20
                                Layout.maximumHeight: 20
                                Layout.preferredHeight: 20
                                Layout.fillWidth: true
                                Layout.fillHeight: false
                            }
                        }
                    }
                }
                Item {
                    Layout.preferredHeight: 40
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                RowLayout {
                    id: navBarView
                    alignment: Qt.LeftToRight | Qt.AlignVCenter
                    margins: 11
                    Layout.preferredWidth: controlPanelLayout.preferredWidth
                    Button {
                        id: btnPevious
                        Layout.alignment: Qt.AlignLeft
                        width: layout.preferredWidth
                        height: layout.preferredHeight
                        Layout.preferredWidth: 85
                        Layout.preferredHeight: 30
                        text: "Previous"
                        font.pointSize: 10
                        font.kerning: true
                    }
                    Button {
                        id: btnBack
                        Layout.alignment: Qt.AlignHCenter
                        width: layout.preferredWidth
                        height: layout.preferredHeight
                        Layout.preferredWidth: 85
                        Layout.preferredHeight: 30
                        text: "Back"
                        font.pointSize: 10
                        font.kerning: true
                    }
                    Button {
                        id: btnNext
                        Layout.alignment: Qt.AlignRight
                        width: layout.preferredWidth
                        height: layout.preferredHeight
                        Layout.preferredWidth: 85
                        Layout.preferredHeight: 30
                        text: "Next"
                        font.pointSize: 10
                        font.kerning: true
                    }
                }
            }
        }
    }
}
