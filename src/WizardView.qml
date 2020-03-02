import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

Item {
    GridLayout {
        id: containerWidget
        anchors.fill: parent
        Layout.fillHeight: true
        Layout.fillWidth: true
        RowLayout {
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true
            ColumnLayout {
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

                spacing: 0

                RowLayout {
                    topMargin: 10
                    spacing: 6

                    StackLayout {
                        id: navStack
                        // welcome page
                        ColumnLayout {
                            id: welcomePage
                            Layout.anchors.fill: true
                            margins: 9
                            spacing: 6

                            RowLayout {
                                Label {
                                    id: welcomeLabel
                                }
                            }

                            RowLayout {
                                alignment: Qt.AlignCenter
                            }
                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnCreateLocalAccount
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnImportFromDevice
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnImportFromBackup
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnShowAdvanced
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnCreateAccountManager
                                }
                            }
                            RowLayout {
                                spacing: 6
                                Button {
                                    id: btnAddNewSIPAccount
                                }
                            }
                        }
                        // create account page

                        // create SIP account page

                        // import form backup page

                        // backup keys page

                        // import form device page

                        // connect to account manage page

                        // spinner page
                    }
                }

                RowLayout {
                    id: navBarView
                    alignment: Qt.LeftToRight | Qt.AlignVCenter
                    margins: 11
                    Button {
                        id: btnPevious
                    }
                    Button {
                        id: btnBack
                    }
                    Button {
                        id: btnNext
                    }
                }
            }
        }
    }
}
