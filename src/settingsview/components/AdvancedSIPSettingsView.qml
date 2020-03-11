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

    // call setting section
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

    // voice mail section
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

    // security section
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

            rowSpacing: 6
            columnSpacing: 6

            rows: 14
            columns: 3

            // First row
            ToggleSwitch{
                id: encryptMediaStreamsToggle

                labelText: qsTr("Encrypt Media Streams(SRTP)")
                fontPointSize: 10

                Layout.row: 0
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 0
                Layout.column: 1
            }

            // second row
            ToggleSwitch{
                id: enableSDESToggle

                labelText: qsTr("Enable SDES(Key Exchange)")
                fontPointSize: 10

                Layout.row: 1
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 1
                Layout.column: 1
            }

            // third row
            ToggleSwitch{
                id: fallbackRTPToggle

                labelText: qsTr("Can Fallback on RTP")
                fontPointSize: 10

                Layout.row: 2
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 2
                Layout.column: 1
            }

            // fourth row
            ToggleSwitch{
                id: encryptNegotitationToggle

                labelText: qsTr("Encrypt Negotiation(TLS)")
                fontPointSize: 10

                Layout.row: 3
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 3
                Layout.column: 1
            }

            // fifth row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 4
                Layout.column: 0

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

                Layout.row: 4
                Layout.column: 1
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

                Layout.row: 4
                Layout.column: 2
            }

            // sixth row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 5
                Layout.column: 0

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

                Layout.row: 5
                Layout.column: 1
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

                Layout.row: 5
                Layout.column: 2
            }

            // seventh row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 6
                Layout.column: 0

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

                Layout.row: 6
                Layout.column: 1
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

                Layout.row: 6
                Layout.column: 2
            }

            // eight row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 7
                Layout.column: 0

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

                Layout.row: 7
                Layout.column: 1
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

                Layout.row: 7
                Layout.column: 2
            }

            // nineth row
            ToggleSwitch{
                id: verifyIncomingCertificatesServerToogle

                labelText: qsTr("Verify Certificates(Server Side)")
                fontPointSize: 10

                Layout.row: 8
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 8
                Layout.column: 1
            }

            // tenth row
            ToggleSwitch{
                id: verifyIncomingCertificatesClientToogle

                labelText: qsTr("Verify Certificates(Client Side)")
                fontPointSize: 10

                Layout.row: 9
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 9
                Layout.column: 1
            }

            //eleventh row
            ToggleSwitch{
                id: requireCeritificateForTLSIncomingToggle

                labelText: qsTr("TLS Connections Require Certificate")
                fontPointSize: 10

                Layout.row: 10
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row:10
                Layout.column: 1
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

                Layout.row: 11
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 11
                Layout.column: 1
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

                Layout.row: 11
                Layout.column: 2
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

                Layout.row: 12
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 12
                Layout.column: 1
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

                Layout.row: 12
                Layout.column: 2
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

                Layout.row: 13
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 13
                Layout.column: 1
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

                Layout.row: 13
                Layout.column: 2
            }
        }
    }

    Item{
            Layout.fillWidth: true

            Layout.maximumHeight: 20
            Layout.preferredHeight: 20
            Layout.minimumHeight: 20
        }

    // connectivity section
    ColumnLayout{
            spacing: 6
            Layout.fillWidth: true

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 27
                Layout.preferredHeight: 27
                Layout.maximumHeight: 27

                text: qsTr("Connectivity")

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

            GridLayout{
                Layout.leftMargin: 20
                Layout.fillWidth: true

                rowSpacing: 6
                columnSpacing: 6

                rows: 9
                columns: 3

                // 1st row
                Label{
                    Layout.minimumWidth: 286
                    Layout.preferredWidth: 286

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Registration Expire Timeout(seconds)")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 0
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20

                    Layout.row: 0
                    Layout.column: 1
                }

                SpinBox{
                    id: registrationExpireTimeoutSpinBox

                    Layout.maximumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.minimumWidth: 250

                    Layout.maximumHeight: 30
                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30

                    Layout.alignment: Qt.AlignCenter

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 3000
                    stepSize: 1

                    Layout.row: 0
                    Layout.column: 2
                }

                // 2nd row
                Label{
                    Layout.minimumWidth: 286
                    Layout.preferredWidth: 286

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Newtwork interface")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 1
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20

                    Layout.row: 1
                    Layout.column: 1
                }

                SpinBox{
                    id: networkInterfaceSpinBox

                    Layout.maximumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.minimumWidth: 250

                    Layout.maximumHeight: 30
                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30

                    Layout.alignment: Qt.AlignCenter

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65536
                    stepSize: 1

                    Layout.row: 1
                    Layout.column: 2
                }

            // 3rd row
            ToggleSwitch{
                id: checkBoxUPnPSIP

                labelText: qsTr("Use UPnP")
                fontPointSize: 10

                Layout.row: 2
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 2
                Layout.column: 1
            }

            // 4th row
            ToggleSwitch{
                id: checkBoxTurnEnableSIP

                labelText: qsTr("Use TURN")
                fontPointSize: 10

                Layout.row: 3
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 3
                Layout.column: 1
            }

            // 5th row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 4
                Layout.column: 0

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 260
                    Layout.preferredWidth: 260
                    Layout.minimumWidth: 260

                    text: qsTr("TURN Address")

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

                Layout.row: 4
                Layout.column: 1
            }

            TextField{
                id: lineEditTurnAddressSIP

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

                Layout.row: 4
                Layout.column: 2
            }

            // 6th row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 5
                Layout.column: 0

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 260
                    Layout.preferredWidth: 260
                    Layout.minimumWidth: 260

                    text: qsTr("TURN Username")

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

                Layout.row: 5
                Layout.column: 1
            }

            TextField{
                id: lineEditTurnUsernameSIP

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

                Layout.row: 5
                Layout.column: 2
            }

            // 7th row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 6
                Layout.column: 0

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 260
                    Layout.preferredWidth: 260
                    Layout.minimumWidth: 260

                    text: qsTr("TURN Password")

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

                Layout.row: 6
                Layout.column: 1
            }

            TextField{
                id: lineEditTurnPsswdSIP

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

                Layout.row: 6
                Layout.column: 2
            }

            // 8th row
            RowLayout{
                spacing: 6
                Layout.maximumHeight: 30

                Layout.row: 7
                Layout.column: 0

                Item{
                    Layout.fillHeight: true

                    Layout.maximumWidth: 20
                    Layout.preferredWidth: 20
                    Layout.minimumWidth: 20
                }

                Label{
                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 260
                    Layout.preferredWidth: 260
                    Layout.minimumWidth: 260

                    text: qsTr("TURN Realm")

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

                Layout.row: 7
                Layout.column: 1
            }

            TextField{
                id: lineEditTurnRealmSIP

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

                Layout.row: 7
                Layout.column: 2
            }

            // 9th row
            ToggleSwitch{
                id: checkBoxSTUNEnableSIP

                labelText: qsTr("Use STUN")
                fontPointSize: 10

                Layout.row: 8
                Layout.column: 0
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 8
                Layout.column: 1
            }

            TextField{
                id: lineEditSTUNAddressSIP

                Layout.alignment: Qt.AlignCenter

                Layout.maximumHeight: 29
                Layout.preferredHeight: 29
                Layout.minimumHeight: 29

                Layout.maximumWidth: 250
                Layout.preferredWidth: 250
                Layout.minimumWidth: 250

                placeholderText: qsTr("STUN Address")

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 8
                Layout.column: 2
            }
            }
        }

        Item{
            Layout.fillWidth: true

            Layout.maximumHeight: 20
            Layout.preferredHeight: 20
            Layout.minimumHeight: 20
        }

        // public address section
        ColumnLayout{
            spacing: 6
            Layout.fillWidth: true

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 27
                Layout.preferredHeight: 27
                Layout.maximumHeight: 27

                text: qsTr("Public Address")

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

            GridLayout{
                Layout.leftMargin: 20
                Layout.fillWidth: true

                rowSpacing: 6
                columnSpacing: 6

                rows: 3
                columns: 3

                // 1st row
                ToggleSwitch{
                    id: checkBoxCustomAddressPort

                    Layout.maximumWidth: 88
                    labelText: qsTr("Use Custom Address/Port")
                    fontPointSize: 10

                    Layout.row: 0
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.row: 0
                    Layout.column: 1
                }

                //2nd row
                Label{
                    Layout.leftMargin: 26

                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 60
                    Layout.preferredWidth: 60
                    Layout.minimumWidth: 60

                    text: qsTr("Address")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 1
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.row: 1
                    Layout.column: 1
                }

                TextField{
                    id: lineEditSIPCustomAddress

                    Layout.maximumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.minimumHeight: 29

                    Layout.maximumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.minimumWidth: 250

                    font.pixelSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment:Text.AlignVCenter

                    Layout.row: 1
                    Layout.column: 2
                }

                //3rd row
                Label{
                    Layout.leftMargin: 26

                    Layout.maximumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.minimumHeight: 27

                    Layout.maximumWidth: 60
                    Layout.preferredWidth: 60
                    Layout.minimumWidth: 60

                    text: qsTr("Port")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 2
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.row: 2
                    Layout.column: 1
                }

                SpinBox{
                    id: customPortSIPSpinBox

                    Layout.maximumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.minimumWidth: 250

                    Layout.maximumHeight: 30
                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30

                    Layout.alignment: Qt.AlignCenter

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65535
                    stepSize: 1

                    Layout.row: 2
                    Layout.column: 2
                }
            }
        }

        // media section
        ColumnLayout{

        }


        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 48
            Layout.preferredHeight: 48
            Layout.minimumHeight: 48
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
}


