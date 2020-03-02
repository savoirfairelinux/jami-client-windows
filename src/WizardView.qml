import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Window {
    id: wizardViewWindow

    property int minWidth: 320
    property int minHeight: 400
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 768
    height: 873
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: wizardViewRect

        anchors.fill: parent

        ColumnLayout {
            id: contentLayout
            //anchors.fill: parent
            width: parent.width * 2 / 3
            height: parent.height * 2 / 3
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            anchors.centerIn: parent.center
            spacing: 40

            StackLayout {
                id: controlPanelStackView
                currentIndex: 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                // welcome page
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
                            font.pointSize: 10
                            font.kerning: true
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
                        }
                    }
                    RowLayout {
                        spacing: 6
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignCenter
                        Button {
                            id: btnAddNewSIPAccount
                            Layout.preferredWidth: 256
                            Layout.preferredHeight: 30
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Add a new SIP account")
                            visible: false
                            font.pointSize: 10
                            font.kerning: true
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
                // spinner Page
                ColumnLayout {
                    id: spinnerPage
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 6

                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    Label {
                        id: spinnerLabel
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumWidth: 200
                        Layout.minimumHeight: 200

                        Layout.maximumWidth: 16777215
                        Layout.maximumHeight: 16777215

                        background: AnimatedImage {
                            playing: true
                            paused: false
                            source: "qrc:/images/jami_eclipse_spinner.gif"
                            fillMode: Image.PreserveAspectFit
                            mipmap: true
                        }
                    }
                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }
                    Label {
                        id: progressLabel
                        Layout.alignment: Qt.AlignHCenter
                        text: "Generating your Jami account"
                        font.pointSize: 11
                        font.kerning: true
                    }
                    Item {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.minimumHeight: 20
                        Layout.maximumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                    }
                }
            }

            RowLayout {
                id: navBarView
                Layout.alignment: Qt.AlignBottom

                //Layout.preferredHeight: 52
                Layout.fillWidth: true
                Layout.fillHeight: false

                Button {
                    id: btnPevious
                    Layout.alignment: Qt.AlignLeft
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Previous")
                    font.pointSize: 10
                    font.kerning: true
                }
                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    id: btnBack
                    Layout.alignment: Qt.AlignHCenter
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Back")
                    font.pointSize: 10
                    font.kerning: true
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Button {
                    id: btnNext
                    Layout.alignment: Qt.AlignRight
                    width: layout.preferredWidth
                    height: layout.preferredHeight
                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Next")
                    font.pointSize: 10
                    font.kerning: true
                }
            }
        }

        //        StackView {
        //            id: wizardControlPanel
        //        }

        //        GridLayout {
        //            id: containerWidget
        //            flow: GridLayout.LeftToRight

        //            anchors.fill: parent
        //            Layout.fillHeight: true
        //            Layout.fillWidth: true
        //            RowLayout {
        //                Layout.fillHeight: true
        //                Layout.fillWidth: true

        //                ColumnLayout {
        //                    id: controlPanelLayout
        //                    Layout.alignment: Qt.AlignCenter

        //                    Layout.maximumWidth: parent.width * (2 / 3)
        //                    Layout.maximumHeight: parent.height * (2 / 3)
        //                    Layout.minimumWidth: parent.width * (1 / 3)
        //                    Layout.minimumHeight: parent.height * (1 / 3)
        //                    Layout.preferredWidth: 512
        //                    Layout.preferredHeight: 638

        //                    topMargin: 0
        //                    bottomMargin: 30
        //                    leftMargin: 30
        //                    rightMargin: 30

        //                    spacing: 40

        //                    RowLayout {
        //                        topMargin: 10
        //                        spacing: 6
        //                        Layout.fillHeight: true
        //                        Layout.fillWidth: true

        //                        StackLayout {
        //                            id: navStack
        //                            currentIndex: 0

        //                            // welcome page
        //                            ColumnLayout {
        //                                id: welcomePage
        //                                Layout.anchors.fill: true
        //                                margins: 9
        //                                spacing: 6
        //                                Item {
        //                                    // put a spacer to make the buttons closs to the middle
        //                                    Layout.minimumHeight: 57
        //                                    Layout.maximumHeight: 57
        //                                    Layout.preferredHeight: 57
        //                                    Layout.fillWidth: true
        //                                }
        //                                RowLayout {
        //                                    Layout.fillWidth: true
        //                                    Label {
        //                                        id: welcomeLabel
        //                                        text: "Welcome to"
        //                                        font.pointSize: 30
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    Layout.fillWidth: true
        //                                    alignment: Qt.AlignHCenter
        //                                    Label {
        //                                        id: welcomeLogo
        //                                        Layout.minimumWidth: 0
        //                                        Layout.minimumHeight: 0
        //                                        Layout.maximumWidth: 16777215
        //                                        Layout.maximumHeight: 16777215
        //                                        background: Image {
        //                                            id: logoIMG
        //                                            source: "qrc:/images/logo-jami-standard-coul.png"
        //                                            fillMode: Image.PreserveAspectFit
        //                                            mipmap: true
        //                                        }
        //                                    }
        //                                }
        //                                Item {
        //                                    // put a spacer to make the buttons closs to the middle
        //                                    Layout.preferredHeight: 57
        //                                    Layout.fillWidth: true
        //                                    Layout.fillHeight: true
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnCreateLocalAccount
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Create local account"
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnImportFromDevice
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Import from device"
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnImportFromBackup
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Import from backup"
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnShowAdvanced
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Show Advanced"
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnCreateAccountManager
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Connect to account manager"
        //                                        visible: false
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                RowLayout {
        //                                    spacing: 6
        //                                    Layout.fillWidth: true
        //                                    Button {
        //                                        id: btnAddNewSIPAccount
        //                                        Layout.preferredWidth: 256
        //                                        Layout.preferredHeight: 30
        //                                        text: "Add a new SIP account"
        //                                        visible: false
        //                                        font.pointSize: 10
        //                                        font.kerning: true
        //                                    }
        //                                }
        //                                Item {
        //                                    // put a spacer to make the buttons closs to the middle
        //                                    Layout.minimumHeight: 65
        //                                    Layout.maximumHeight: 65
        //                                    Layout.preferredHeight: 65
        //                                    Layout.fillWidth: true
        //                                }
        //                            }
        //                            // create account page

        //                            // create SIP account page

        //                            // import form backup page

        //                            // backup keys page

        //                            // import form device page

        //                            // connect to account manage page

        //                            // spinner page
        //                            ColumnLayout {
        //                                id: spinnerPage
        //                                StackLayout.count: 1

        //                                Layout.anchors.fill: true
        //                                margins: 9
        //                                spacing: 6
        //                                alignment: Qt.AlignHCenter

        //                                Item {
        //                                    Layout.preferredHeight: 40
        //                                    Layout.fillWidth: true
        //                                    Layout.fillHeight: true
        //                                }
        //                                Label {
        //                                    id: spinnerLabel
        //                                    Layout.minimumWidth: 0
        //                                    Layout.minimumHeight: 0

        //                                    Layout.maximumWidth: 16777215
        //                                    Layout.maximumHeight: 16777215

        //                                    background: Image {
        //                                        source: "qrc:/images/jami_eclipse_spinner.gif"
        //                                    }
        //                                }
        //                                Item {
        //                                    Layout.preferredHeight: 40
        //                                    Layout.fillWidth: true
        //                                    Layout.fillHeight: true
        //                                }
        //                                Label {
        //                                    id: progressLabel
        //                                    text: "Generating your Jami account"
        //                                    font.pointSize: 11
        //                                    font.kerning: true
        //                                }
        //                                Item {
        //                                    Layout.minimumHeight: 20
        //                                    Layout.maximumHeight: 20
        //                                    Layout.preferredHeight: 20
        //                                    Layout.fillWidth: true
        //                                    Layout.fillHeight: false
        //                                }
        //                            }
        //                        }
        //                    }
        //                    Item {
        //                        Layout.preferredHeight: 40
        //                        Layout.fillWidth: true
        //                        Layout.fillHeight: true
        //                    }
        //                    RowLayout {
        //                        id: navBarView
        //                        alignment: Qt.LeftToRight | Qt.AlignVCenter
        //                        margins: 11
        //                        Layout.preferredWidth: controlPanelLayout.preferredWidth
        //                        Button {
        //                            id: btnPevious
        //                            Layout.alignment: Qt.AlignLeft
        //                            width: layout.preferredWidth
        //                            height: layout.preferredHeight
        //                            Layout.preferredWidth: 85
        //                            Layout.preferredHeight: 30
        //                            text: "Previous"
        //                            font.pointSize: 10
        //                            font.kerning: true
        //                        }
        //                        Button {
        //                            id: btnBack
        //                            Layout.alignment: Qt.AlignHCenter
        //                            width: layout.preferredWidth
        //                            height: layout.preferredHeight
        //                            Layout.preferredWidth: 85
        //                            Layout.preferredHeight: 30
        //                            text: "Back"
        //                            font.pointSize: 10
        //                            font.kerning: true
        //                        }
        //                        Button {
        //                            id: btnNext
        //                            Layout.alignment: Qt.AlignRight
        //                            width: layout.preferredWidth
        //                            height: layout.preferredHeight
        //                            Layout.preferredWidth: 85
        //                            Layout.preferredHeight: 30
        //                            text: "Next"
        //                            font.pointSize: 10
        //                            font.kerning: true
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //    }
    }
}
