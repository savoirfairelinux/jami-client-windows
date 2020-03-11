import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Rectangle {
    anchors.fill: parent

    RowLayout {
        anchors.fill: parent
        spacing: 0
        Item {
            Layout.fillHeight: true
            Layout.maximumWidth: 48
            Layout.preferredWidth: 48
            Layout.minimumWidth: 48
        }

        ColumnLayout {
            spacing: 7

            Layout.fillHeight: true
            Layout.maximumWidth: 580
            Layout.preferredWidth: 580
            Layout.minimumWidth: 580

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 10
                Layout.preferredHeight: 10
                Layout.maximumHeight: 10
            }

            Label {
                Layout.fillWidth: true
                Layout.minimumHeight: 25
                Layout.preferredHeight: 25
                Layout.maximumHeight: 25

                text: qsTr("Audio / Video")
                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 24
                Layout.preferredHeight: 24
                Layout.maximumHeight: 24
            }

            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("Audio")
                    font.pointSize: 13
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                ColumnLayout {
                    spacing: 7
                    Layout.fillWidth: true

                    RowLayout {
                        spacing: 7
                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.maximumWidth: 77
                            Layout.preferredWidth: 77
                            Layout.minimumWidth: 77

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Microphone")
                            font.pointSize: 11
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        ComboBox {
                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true
                        }
                    }

                    // the audio level meter
                    ProgressBar {
                        Layout.leftMargin: 20

                        Layout.fillWidth: true

                        Layout.minimumHeight: 10
                        Layout.preferredHeight: 10
                        Layout.maximumHeight: 10
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 5
                        Layout.preferredHeight: 5
                        Layout.maximumHeight: 5
                    }

                    RowLayout {
                        spacing: 7
                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.maximumWidth: 95
                            Layout.preferredWidth: 95
                            Layout.minimumWidth: 95

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Output Device")
                            font.pointSize: 11
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        ComboBox {
                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true

                Layout.minimumHeight: 20
                Layout.preferredHeight: 20
                Layout.maximumHeight: 20
            }

            ColumnLayout {
                spacing: 7
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    text: qsTr("Video")
                    font.pointSize: 13
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                ColumnLayout {
                    spacing: 6
                    RowLayout {
                        spacing: 7
                        Layout.fillWidth: true
                        Layout.leftMargin: 20
                        Layout.maximumHeight: 30

                        Label {
                            Layout.maximumWidth: 44
                            Layout.preferredWidth: 44
                            Layout.minimumWidth: 44

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Device")
                            font.pointSize: 11
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true
                        }
                    }

                    RowLayout {
                        spacing: 7
                        Layout.fillWidth: true
                        Layout.leftMargin: 20
                        Layout.maximumHeight: 30

                        Label {
                            Layout.maximumWidth: 47
                            Layout.preferredWidth: 47
                            Layout.minimumWidth: 47

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            text: qsTr("Device")
                            font.pointSize: 11
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        ComboBox {
                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 20
                Layout.preferredHeight: 20
                Layout.maximumHeight: 20
            }

            Rectangle {
                Layout.alignment: Qt.AlignHCenter

                Layout.maximumWidth: 580
                Layout.preferredWidth: 580
                Layout.minimumWidth: 580

                Layout.minimumHeight: 224
                Layout.preferredHeight: 224
                Layout.maximumHeight: 224

                color: "black"
            }

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                text: qsTr("Preview unavailable")
                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            // Toggle switch to enable hardware acceleration
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
