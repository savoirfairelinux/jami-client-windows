import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import "../../commoncomponents"

Rectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true

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
            spacing: 6

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

                text: qsTr("General")
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

            // system setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("System")
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
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: enableDestopNotificationSwitch

                        Layout.leftMargin: 20

                        labelText: "Enable desktop notifications"
                        fontPointSize: 11
                    }

                    ToggleSwitch {
                        id: keepMinimizeOnCloseSwitch

                        Layout.leftMargin: 20

                        labelText: "Keep minimize on close"
                        fontPointSize: 11
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 94
                            Layout.preferredWidth: 94
                            Layout.minimumWidth: 94

                            text: qsTr("Download folder")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        HoverableButton {
                            id: downloadButton

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            icon.source: "qrc:/images/icons/round-folder-24px.svg"
                            icon.height: 24
                            icon.width: 24

                            text: ""
                            fontPointSize: 10
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

            // call recording setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("Call Recording")
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
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: alwaysRecordCallSwitch

                        Layout.leftMargin: 20

                        labelText: "Always record calls"
                        fontPointSize: 11
                    }

                    ToggleSwitch {
                        id: recordPreviewVideoForACallSwitch

                        Layout.leftMargin: 20

                        labelText: "Record preview video for a call"
                        fontPointSize: 11
                    }

                    RowLayout {
                        spacing: 6
                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 42
                            Layout.preferredWidth: 42
                            Layout.minimumWidth: 42

                            text: qsTr("Quality")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        ColumnLayout {
                            spacing: 0
                            Layout.fillHeight: true

                            Layout.maximumWidth: valueLabel.width
                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }

                            Label {
                                id: valueLabel

                                Layout.minimumWidth: 40

                                Layout.minimumHeight: 16
                                Layout.preferredHeight: 16
                                Layout.maximumHeight: 16

                                text: qsTr("VALUE ")

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }

                        Slider {
                            id: recordQualitySlider

                            Layout.fillHeight: true

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            from: 0
                            to: 500
                            stepSize: 1
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 42
                            Layout.preferredWidth: 42
                            Layout.minimumWidth: 42

                            text: qsTr("Save in")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        HoverableButton {
                            id: recordPathButton

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            icon.source: "qrc:/images/icons/round-folder-24px.svg"
                            icon.height: 24
                            icon.width: 24

                            text: ""
                            fontPointSize: 10
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

            // update setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("Updates")
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
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: autoUpdateSwitch

                        Layout.leftMargin: 20

                        labelText: "Check for updates automatically"
                        fontPointSize: 11
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        HoverableButton {
                            id: checkUpdateButton

                            Layout.maximumWidth: 275
                            Layout.preferredWidth: 275
                            Layout.minimumWidth: 275

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: "Check for updates now"
                            fontPointSize: 10
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        HoverableButton {
                            id: installBetaButton

                            Layout.maximumWidth: 275
                            Layout.preferredWidth: 275
                            Layout.minimumWidth: 275

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: "Install the latest beta version"
                            fontPointSize: 10
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            // spacer on the bottom
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
