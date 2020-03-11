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

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true
            Layout.leftMargin: 20

            ToggleSwitch {
                id: checkBoxAutoAnswerSIP
                labelText: qsTr("Auto Answer Call")
                fontPointSize: 10

                Layout.leftMargin: 20
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.maximumHeight: 30

                ToggleSwitch {
                    id: checkBoxCustomRingtoneSIP
                    labelText: qsTr("Enable Custom Ringtone")
                    fontPointSize: 10

                    Layout.maximumWidth: 164
                    Layout.preferredWidth: 164
                    Layout.minimumWidth: 164
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                HoverableButton {
                    id: btnRingtoneSIP

                    radius: height / 2

                    Layout.maximumWidth: 164
                    Layout.preferredWidth: 164
                    Layout.minimumWidth: 164

                    Layout.maximumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16
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
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true

            Layout.minimumHeight: 27
            Layout.preferredHeight: 27
            Layout.maximumHeight: 27

            text: qsTr("Voicemail")

            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true

            Layout.minimumHeight: 13
            Layout.preferredHeight: 13
            Layout.maximumHeight: 13
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.maximumHeight: 30

            Label {
                Layout.maximumWidth: 162
                Layout.preferredWidth: 162
                Layout.minimumWidth: 162

                Layout.minimumHeight: 28
                Layout.preferredHeight: 28
                Layout.maximumHeight: 28

                text: qsTr("Voicemail Dial Code")
                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            TextField {
                id: lineEditVoiceMailDialCode

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
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

            text: qsTr("Security")

            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        GridLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            rows: 14
            columns: 3
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
