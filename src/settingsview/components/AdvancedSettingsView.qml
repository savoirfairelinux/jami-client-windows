import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

import "../../commoncomponents"

ColumnLayout {
    spacing: 6
    Layout.fillWidth: true

    Item {
        Layout.fillWidth: true

        Layout.minimumHeight: 24
        Layout.preferredHeight: 24
        Layout.maximumHeight: 24
    }

    ColumnLayout {
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Call Settings")
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
                id: checkBoxUntrusted

                Layout.fillWidth: true
                Layout.leftMargin: 20

                labelText: qsTr("Allow Calls From Unstructed Peers")
                fontPointSize: 10
            }

            ToggleSwitch {
                id: checkBoxAutoAnswer

                Layout.fillWidth: true
                Layout.leftMargin: 20

                labelText: qsTr("Auto Answer Calls")
                fontPointSize: 10
            }

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.maximumHeight: 30

                ToggleSwitch {
                    id: checkBoxCustomRingtone

                    labelText: qsTr("Enable Custom Ringtone")
                    fontPointSize: 10
                }

                HoverableButton {
                    id: btnRingtone

                    Layout.minimumWidth: 300
                    Layout.preferredWidth: 300
                    Layout.maximumWidth: 300

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    radius: height / 2

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
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
            spacing: 6
            Layout.fillWidth: true

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 27
                Layout.preferredHeight: 27
                Layout.maximumHeight: 27

                text: qsTr("Name Server")
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

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.maximumHeight: 29

                Label {}

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                TextField {
                    id: lineEditNameServer

                    Layout.minimumWidth: 300
                    Layout.preferredWidth: 300
                    Layout.maximumWidth: 300

                    Layout.minimumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.maximumHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}
