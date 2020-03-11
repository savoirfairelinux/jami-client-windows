import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import "../../commoncomponents"

Rectangle {
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 10
            Layout.minimumHeight: 10
            Layout.preferredHeight: 10

            Layout.alignment: Qt.AlignTop
        }

        RowLayout {
            spacing: 6

            Layout.alignment: Qt.AlignTop

            Layout.fillWidth: true
            Layout.maximumHeight: 30
            Layout.minimumHeight: 0
            Layout.preferredHeight: accountPageTitle.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            Label {
                id: accountPageTitle

                Layout.preferredWidth: 117

                Layout.maximumHeight: 25
                Layout.preferredHeight: 25
                Layout.minimumHeight: 25

                text: qsTr("Jami Account")

                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
        ScrollView {
            id: scrowArea

            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            font.pointSize: 8
            font.kerning: true

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: 622

                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 24
                        Layout.preferredWidth: 24
                        Layout.minimumWidth: 24
                    }

                    ToggleSwitch {
                        id: accountEnableToggle

                        labelText: qsTr("Enable")
                        fontPointSize: 11
                    }

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.minimumWidth: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 21
                            Layout.preferredHeight: 21
                            Layout.minimumHeight: 21

                            text: qsTr("Profile")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 10
                            Layout.preferredHeight: 10
                            Layout.minimumHeight: 10
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            alignment: Qt.AlignLeft

                            spacing: 6

                            Rectangle {
                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261

                                Layout.minimumHeight: 261
                                Layout.preferredHeight: 261
                                Layout.maximumHeight: 261

                                Layout.leftMargin: 20

                                color: "blue"
                            }

                            TextField {
                                id: displayNameLineEdit

                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.minimumWidth: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 21
                            Layout.preferredHeight: 21
                            Layout.minimumHeight: 21

                            text: qsTr("Identity")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 10
                            Layout.preferredWidth: 10
                            Layout.minimumWidth: 10
                        }

                        ColumnLayout {
                            spacing: 7

                            Layout.fillWidth: true

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Label {
                                    Layout.maximumWidth: 13
                                    Layout.preferredWidth: 13
                                    Layout.minimumWidth: 13

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Id")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                TextField {
                                    Layout.fillWidth: true

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 32

                                alignment: Qt.AlignVCenter

                                Label {
                                    id: lblRegisteredName

                                    Layout.maximumWidth: 97
                                    Layout.preferredWidth: 97
                                    Layout.minimumWidth: 97

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Registered name")

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                RowLayout {
                                    spacing: 6
                                    Layout.fillWidth: true
                                    Layout.maximumHeight: 30
                                    Layout.alignment: Qt.AlignVCenter

                                    TextField {
                                        id: currentRegisteredID

                                        Layout.maximumWidth: 300
                                        Layout.preferredWidth: 300
                                        Layout.minimumWidth: 300

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30

                                        placeholderText: qsTr("Type here to register a username")

                                        font.pointSize: 10
                                        font.kerning: true

                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    Label {
                                        id: lookupStatusLabel
                                    }

                                    HoverableButton {
                                        id: btnRegisterName
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
