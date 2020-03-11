import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.AccountAdapter 1.0
import net.jami.NameDirectory 1.0
import net.jami.UtilsAdapter 1.0
import net.jami.AVModel 1.0
import net.jami.PreviewRenderer 1.0

Rectangle {
    function startPreviewing(force = false){
        UtilsAdapter.startPreviewing(force)
        previewAvailable = true
    }

    function stopPreviewing(){
        if(!UtilsAdapter.hasVideoCall()) {
            UtilsAdapter.stopPreviewing()
        }
        previewAvailable = false
    }

    function startAudioMeter(async = true){
        audioInputMeter.start()
        AccountAdapter.startAudioMeter(async)
    }

    function stopAudioMeter(async = true){
        AccountAdapter.stopAudioMeter(async)
    }

    function slotAudioMeter(id, level){
        if (id === "audiolayer_id") {
                audioInputMeter.setLevel(level)
            }
    }

    function slotSetHardwareAccel(state){
        AccountAdapter.avModel().setHardwareAcceleration(state)
    }

    property bool previewAvailable: false

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
                            id: inputComboBox

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
                    LevelMeter {
                        id: audioInputMeter

                        Layout.leftMargin: 20

                        Layout.fillWidth: true

                        Layout.minimumHeight: 10
                        Layout.preferredHeight: 10
                        Layout.maximumHeight: 10

                        indeterminate: false
                        from: 0
                        to: 100
                        value: -1
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
                            id: outputComboBox

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
                            id: deviceBox

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

                            text: qsTr("Format")
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
                            id: formatBox

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

            RowLayout{
                Layout.alignment: Qt.AlignHCenter

                Layout.preferredWidth: 580
                Layout.minimumWidth: 580

                Layout.minimumHeight: 224
                Layout.preferredHeight: 224
                Layout.maximumHeight: 224

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.minimumWidth: 580

                    color: "black"
                    PreviewRenderer{
                        id: peviewWidget

                        visible: previewAvailable
                        height: parent.height
                        width: 224
                        x: (parent.width - width) /2
                        y: 0
                    }
                }
            }

            Label {
                visible: !previewAvailable

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
            ToggleSwitch {
                id: hardwareAccelControl

                labelText: "Enable hardware acceleration"

                onCheckedChanged: {
                    slotSetHardwareAccel(checked)
                }
            }

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
