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

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 13
            Layout.preferredHeight: 13
            Layout.minimumHeight: 13
        }

        GridLayout {
            Layout.leftMargin: 20
            Layout.fillWidth: true

            rows: 14
            columns: 3

            // First row
            ToggleSwitch{
                id: encryptMediaStreamsToggle

                labelText: qsTr("Encrypt Media Streams(SRTP)")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // second row
            ToggleSwitch{
                id: enableSDESToggle

                labelText: qsTr("Enable SDES(Key Exchange)")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // third row
            ToggleSwitch{
                id: fallbackRTPToggle

                labelText: qsTr("Can Fallback on RTP")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // fourth row
            ToggleSwitch{
                id: encryptNegotitationToggle

                labelText: qsTr("Encrypt Negotiation(TLS)")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // fifth row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30

                    Layout.maximumWidth: 209
                    Layout.preferredWidth: 209
                    Layout.minimumWidth: 209

                    text: qsTr("CA Certificate")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            HoverableButton{
                id: btnSIPCACert

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                radius: height / 2

                icon.source: "qrc:/images/icons/round-folder-24px.svg"
                icon.width: 16
                icon.height: 16
            }

            // sixth row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30

                    Layout.maximumWidth: 209
                    Layout.preferredWidth: 209
                    Layout.minimumWidth: 209

                    text: qsTr("User Certificate")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            HoverableButton{
                id: btnSIPUserCert

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                radius: height / 2

                icon.source: "qrc:/images/icons/round-folder-24px.svg"
                icon.width: 16
                icon.height: 16
            }

            // seventh row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30

                    Layout.maximumWidth: 209
                    Layout.preferredWidth: 209
                    Layout.minimumWidth: 209

                    text: qsTr("Private Key")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            HoverableButton{
                id: btnSIPPrivateKey

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                radius: height / 2

                icon.source: "qrc:/images/icons/round-folder-24px.svg"
                icon.width: 16
                icon.height: 16
            }

            // eight row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.minimumHeight: 30

                    Layout.maximumWidth: 209
                    Layout.preferredWidth: 209
                    Layout.minimumWidth: 209

                    text: qsTr("Private Key Password")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            TextField{
                id: lineEditSIPCertPassword

                Layout.alignment: Qt.AlignCenter

                Layout.maximumHeight: 29
                Layout.preferredHeight: 29
                Layout.minimumHeight: 29

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            // nineth row
            ToggleSwitch{
                id: verifyIncomingCertificatesServerToogle

                labelText: qsTr("Verify Certificates(Server Side)")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // tenth row
            ToggleSwitch{
                id: verifyIncomingCertificatesClientToogle

                labelText: qsTr("Verify Certificates(Client Side)")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            //eleventh row
            ToggleSwitch{
                id: requireCeritificateForTLSIncomingToggle

                labelText: qsTr("TLS Connections Require Certificate")
                fontPointSize: 10
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            // twelveth row
            Label{
                Layout.fillWidth: true

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                text: qsTr("TLS Protocol Method")
                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            ComboBox{
                id:tlsProtocolComboBox

                Layout.maximumWidth: 252
                Layout.preferredWidth: 252
                Layout.minimumWidth: 252

                Layout.maximumHeight: 29
                Layout.minimumHeight: 29
                Layout.preferredHeight: 29

                Layout.alignment: Qt.AlignCenter

                font.pointSize: 10
                font.kerning: true
            }

            // 13th row
            Label{
                Layout.fillWidth: true

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                text: qsTr("Outgoing TLS Server Name")
                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            TextField{
                id: outgoingTLSServerNameLineEdit

                Layout.alignment: Qt.AlignCenter

                Layout.maximumHeight: 29
                Layout.preferredHeight: 29
                Layout.minimumHeight: 29

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            // 14th row
            Label{
                Layout.fillWidth: true

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                text: qsTr("Negotiation Timeout(seconds)")
                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20
            }

            SpinBox{
                id:negotiationTimeoutSpinBox

                Layout.maximumWidth: 252
                Layout.preferredWidth: 252
                Layout.minimumWidth: 252

                Layout.maximumHeight: 30
                Layout.minimumHeight: 30
                Layout.preferredHeight: 30

                Layout.alignment: Qt.AlignCenter

                font.pointSize: 10
                font.kerning: true

                from: 0
                to: 3000
                stepSize: 1
            }
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}
