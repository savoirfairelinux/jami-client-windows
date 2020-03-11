/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3
import net.jami.Models 1.0

import "../../commoncomponents"

ColumnLayout {
    function updateAccountInfoDisplayedAdvanceSIP(){
        // Call Settings
        checkBoxAutoAnswerSIP.checked = SettingsAdaptor.getAccountConfig_AutoAnswer()
        checkBoxCustomRingtoneSIP.checked = SettingsAdaptor.getAccountConfig_Ringtone_RingtoneEnabled()

        // security
        btnSIPCACert.enabled = SettingsAdaptor.getAccountConfig_TLS_Enable()
        btnSIPUserCert.enabled = SettingsAdaptor.getAccountConfig_TLS_Enable()
        btnSIPPrivateKey.enabled = SettingsAdaptor.getAccountConfig_TLS_Enable()
        lineEditSIPCertPassword.enabled = SettingsAdaptor.getAccountConfig_TLS_Enable()
        enableSDESToggle.enabled = SettingsAdaptor.getAccountConfig_SRTP_Enabled()
        fallbackRTPToggle.enabled = SettingsAdaptor.getAccountConfig_SRTP_Enabled()

        btnSIPCACert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateListFile())
        btnSIPUserCert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateFile())
        btnSIPPrivateKey.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_PrivateKeyFile())
        lineEditSIPCertPassword.text = SettingsAdaptor.getAccountConfig_TLS_Password()

        encryptMediaStreamsToggle.checked = SettingsAdaptor.getAccountConfig_SRTP_Enabled()
        enableSDESToggle.checked = (SettingsAdaptor.getAccountConfig_SRTP_KeyExchange()  === Account.KeyExchangeProtocol.SDES)
        fallbackRTPToggle.checked = SettingsAdaptor.getAccountConfig_SRTP_RtpFallback()
        encryptNegotitationToggle.checked = SettingsAdaptor.getAccountConfig_TLS_Enable()
        verifyIncomingCertificatesServerToogle.checked = SettingsAdaptor.getAccountConfig_TLS_VerifyServer()
        verifyIncomingCertificatesClientToogle.checked = SettingsAdaptor.getAccountConfig_TLS_VerifyClient()
        requireCeritificateForTLSIncomingToggle.checked = SettingsAdaptor.getAccountConfig_TLS_RequireClientCertificate()

        var method = SettingsAdaptor.getAccountConfig_TLS_Method_inInt()
        tlsProtocolComboBox.currentIndex = method

        outgoingTLSServerNameLineEdit.text = SettingsAdaptor.getAccountConfig_TLS_Servername()
        negotiationTimeoutSpinBox.value = SettingsAdaptor.getAccountConfig_TLS_NegotiationTimeoutSec()

        // Connectivity
        checkBoxUPnPSIP.checked = SettingsAdaptor.getAccountConfig_UpnpEnabled()
        checkBoxTurnEnableSIP.checked = SettingsAdaptor.getAccountConfig_TURN_Enabled()
        lineEditTurnAddressSIP.text = SettingsAdaptor.getAccountConfig_TURN_Server()
        lineEditTurnUsernameSIP.text = SettingsAdaptor.getAccountConfig_TURN_Username()
        lineEditTurnPsswdSIP.text = SettingsAdaptor.getAccountConfig_TURN_Password()
        lineEditTurnRealmSIP.text = SettingsAdaptor.getAccountConfig_TURN_Realm()
        lineEditTurnAddressSIP.enabled = SettingsAdaptor.getAccountConfig_TURN_Enabled()
        lineEditTurnUsernameSIP.enabled = SettingsAdaptor.getAccountConfig_TURN_Enabled()
        lineEditTurnPsswdSIP.enabled = SettingsAdaptor.getAccountConfig_TURN_Enabled()
        lineEditTurnRealmSIP.enabled = SettingsAdaptor.getAccountConfig_TURN_Enabled()

        checkBoxSTUNEnableSIP.checked = SettingsAdaptor.getAccountConfig_STUN_Enabled()
        lineEditSTUNAddressSIP.text = SettingsAdaptor.getAccountConfig_STUN_Server()
        lineEditSTUNAddressSIP.enabled = SettingsAdaptor.getAccountConfig_STUN_Enabled()

        registrationExpireTimeoutSpinBox.value = SettingsAdaptor.getAccountConfig_Registration_Expire()
        networkInterfaceSpinBox.value = SettingsAdaptor.getAccountConfig_Localport()

        // published address
        checkBoxCustomAddressPort.checked = SettingsAdaptor.getAccountConfig_PublishedSameAsLocal()
        lineEditSIPCustomAddress.text = SettingsAdaptor.getAccountConfig_PublishedAddress()
        customPortSIPSpinBox.value = SettingsAdaptor.getAccountConfig_PublishedPort()

        // codecs
        videoCheckBoxSIP.checked = SettingsAdaptor.getAccountConfig_Video_Enabled()
        updateAudioCodecs()
        updateVideoCodecs()
        btnRingtoneSIP.enabled = SettingsAdaptor.getAccountConfig_Ringtone_RingtoneEnabled()
        btnRingtoneSIP.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath())
        lineEditSTUNAddressSIP.enabled = SettingsAdaptor.getAccountConfig_STUN_Enabled()

        // SDP session negotiation ports
        audioRTPMinPortSpinBox.value = SettingsAdaptor.getAccountConfig_Audio_AudioPortMin()
        audioRTPMaxPortSpinBox.value = SettingsAdaptor.getAccountConfig_Audio_AudioPortMax()
        videoRTPMinPortSpinBox.value = SettingsAdaptor.getAccountConfig_Video_VideoPortMin()
        videoRTPMaxPortSpinBox.value = SettingsAdaptor.getAccountConfig_Video_VideoPortMax()

        // voicemail
        lineEditVoiceMailDialCode.text = SettingsAdaptor.getAccountConfig_Mailbox()
    }

    function updateAudioCodecs(){
        audioCodecListModelSIP.layoutAboutToBeChanged()
        audioCodecListModelSIP.dataChanged(audioCodecListModelSIP.index(0, 0),
                                     audioCodecListModelSIP.index(audioCodecListModelSIP.rowCount() - 1, 0))
        audioCodecListModelSIP.layoutChanged()
    }

    function updateVideoCodecs(){
        videoCodecListModelSIP.layoutAboutToBeChanged()
        videoCodecListModelSIP.dataChanged(videoCodecListModelSIP.index(0, 0),
                                     videoCodecListModelSIP.index(videoCodecListModelSIP.rowCount() - 1, 0))
        videoCodecListModelSIP.layoutChanged()
    }

    function decreaseAudioCodecPriority(){
        var index = audioListWidgetSIP.currentIndex
        var codecId = audioCodecListModelSIP.data(audioCodecListModelSIP.index(index,0), AudioCodecListModel.AudioCodecID)

        SettingsAdaptor.decreaseAudioCodecPriority(codecId)
        audioListWidgetSIP.currentIndex = index + 1
        updateAudioCodecs()
    }

    function increaseAudioCodecPriority(){
        var index = audioListWidgetSIP.currentIndex
        var codecId = audioCodecListModelSIP.data(audioCodecListModelSIP.index(index,0), AudioCodecListModel.AudioCodecID)

        SettingsAdaptor.increaseAudioCodecPriority(codecId)
        audioListWidgetSIP.currentIndex = index - 1
        updateAudioCodecs()
    }

    function decreaseVideoCodecPriority(){
        var index = videoListWidgetSIP.currentIndex
        var codecId = videoCodecListModelSIP.data(videoCodecListModelSIP.index(index,0), VideoCodecListModel.VideoCodecID)

        SettingsAdaptor.decreaseVideoCodecPriority(codecId)
        videoListWidgetSIP.currentIndex = index + 1
        updateVideoCodecs()
    }

    function increaseVideoCodecPriority(){
        var index = videoListWidgetSIP.currentIndex
        var codecId = videoCodecListModelSIP.data(videoCodecListModelSIP.index(index,0), VideoCodecListModel.VideoCodecID)

        SettingsAdaptor.increaseVideoCodecPriority(codecId)
        videoListWidgetSIP.currentIndex = index - 1
        updateVideoCodecs()
    }

    VideoCodecListModel{
        id: videoCodecListModelSIP
    }

    AudioCodecListModel{
        id: audioCodecListModelSIP
    }


    // slots
    function audioRTPMinPortSpinBoxEditFinished(value){
        if (SettingsAdaptor.getAccountConfig_Audio_AudioPortMax() < value) {
            audioRTPMinPortSpinBox.value = SettingsAdaptor.getAccountConfig_Audio_AudioPortMin()
            return
        }
        SettingsAdaptor.audioRTPMinPortSpinBoxEditFinished(value)
    }

    function audioRTPMaxPortSpinBoxEditFinished(value){
        if (value < SettingsAdaptor.getAccountConfig_Audio_AudioPortMin()) {
            audioRTPMaxPortSpinBox.value = SettingsAdaptor.getAccountConfig_Audio_AudioPortMax()
            return
        }
        SettingsAdaptor.audioRTPMaxPortSpinBoxEditFinished(value)
    }

    function videoRTPMinPortSpinBoxEditFinished(value){
        if (SettingsAdaptor.getAccountConfig_Video_VideoPortMax() < value) {
            videoRTPMinPortSpinBox.value = SettingsAdaptor.getAccountConfig_Video_VideoPortMin()
            return
        }
        SettingsAdaptor.videoRTPMinPortSpinBoxEditFinished(value)
    }

    function videoRTPMaxPortSpinBoxEditFinished(value){
        if (value < SettingsAdaptor.getAccountConfig_Video_VideoPortMin()) {
            videoRTPMinPortSpinBox.value = SettingsAdaptor.getAccountConfig_Video_VideoPortMin()
            return
        }
        SettingsAdaptor.videoRTPMaxPortSpinBoxEditFinished(value)
    }


    function changeRingtonePath(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_RingtonePath(url)
            btnRingtoneSIP.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath())
        } else if (SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath().length === 0){
            btnRingtoneSIP.text = qsTr("Add a custom ringtone")
        }
    }

    function changeFileCACert(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FileCACert(url)
            btnSIPCACert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateListFile())
        }
    }

    function changeFileUserCert(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FileUserCert(url)
            btnSIPUserCert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateFile())
        }
    }

    function changeFilePrivateKey(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FilePrivateKey(url)
            btnSIPPrivateKey.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_PrivateKeyFile())
        }
    }

    FileDialog {
        id: ringtonePath_Dialog_SIP

        property string oldPath : SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath()
        property string openPath : oldPath === "" ? (UtilsAdapter.getCurrentPath() + "/ringtones/") : (UtilsAdapter.toFileAbsolutepath(oldPath))

        selectExisting: true
        selectMultiple: false
        title: qsTr("Select a new ringtone")
        folder: openPath

        nameFilters: [qsTr("Audio Files") + " (*.wav *.ogg *.opus *.mp3 *.aiff *.wma)", qsTr(
                "All files") + " (*)"]

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }

        onAccepted: {
            var url = fileUrl.replace("file:///", "")
            changeRingtonePath(url)
        }
    }

    FileDialog {
        id: caCert_Dialog_SIP

        property string oldPath : SettingsAdaptor.getAccountConfig_TLS_CertificateListFile()
        property string openPath : oldPath === "" ? (UtilsAdapter.getCurrentPath() + "/ringtones/") : (UtilsAdapter.toFileAbsolutepath(oldPath))

        selectExisting: true
        selectMultiple: false
        title: qsTr("Select a CA certificate")
        folder: openPath
        nameFilters: [qsTr("Certificate File") + " (*.crt)", qsTr(
                "All files") + " (*)"]

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }

        onAccepted: {
            var url = fileUrl.replace("file:///", "")
            changeFileCACert(url)
        }
    }

    FileDialog {
        id: userCert_Dialog_SIP

        property string oldPath : SettingsAdaptor.getAccountConfig_TLS_CertificateFile()
        property string openPath : oldPath === "" ? (UtilsAdapter.getCurrentPath() + "/ringtones/") : (UtilsAdapter.toFileAbsolutepath(oldPath))

        selectExisting: true
        selectMultiple: false
        title: qsTr("Select a user certificate")
        folder: openPath
        nameFilters: [qsTr("Certificate File") + " (*.crt)", qsTr(
                "All files") + " (*)"]

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }

        onAccepted: {
            var url = fileUrl.replace("file:///", "")
            changeFileUserCert(url)
        }
    }

    FileDialog {
        id: privateKey_Dialog_SIP

        property string oldPath : SettingsAdaptor.getAccountConfig_TLS_PrivateKeyFile()
        property string openPath : oldPath === "" ? (UtilsAdapter.getCurrentPath() + "/ringtones/") : (UtilsAdapter.toFileAbsolutepath(oldPath))

        selectExisting: true
        selectMultiple: false
        title: qsTr("Select a private key")
        folder: openPath
        nameFilters: [qsTr("Key File") + " (*.key)", qsTr(
                "All files") + " (*)"]

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }

        onAccepted: {
            var url = fileUrl.replace("file:///", "")
            changeFilePrivateKey(url)
        }
    }

    spacing: 6
    Layout.preferredWidth: 532
    Layout.maximumWidth: 532

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

                onSwitchToggled: {
                    SettingsAdaptor.setAutoAnswerCalls(checked)
                }
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

                    onSwitchToggled: {
                        SettingsAdaptor.setEnableRingtone(checked)
                        btnRingtoneSIP.enabled = checked
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                HoverableRadiusButton {
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

                    onClicked: {
                        ringtonePath_Dialog_SIP.open()
                    }
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

            InfoLineEdit {
                id: lineEditVoiceMailDialCode

                fieldLayoutWidth: 250

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                onEditingFinished: {
                    SettingsAdaptor.lineEditVoiceMailDialCodeEditFinished(text)
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

                onSwitchToggled: {
                    SettingsAdaptor.setUseSRTP(checked)
                    enableSDESToggle.enabled = checked
                    fallbackRTPToggle.enabled = checked
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setUseSDES(checked)
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setUseRTPFallback(checked)
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setUseTLS(checked)
                    btnSIPCACert.enabled = checked
                    btnSIPUserCert.enabled = checked
                    btnSIPPrivateKey.enabled = checked
                    lineEditSIPCertPassword.enabled = checked
                }
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

            HoverableRadiusButton{
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

                onClicked: {
                    caCert_Dialog_SIP.open()
                }
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

            HoverableRadiusButton{
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

                onClicked: {
                    userCert_Dialog_SIP.open()
                }
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

            HoverableRadiusButton{
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

                onClicked: {
                    privateKey_Dialog_SIP.open()
                }
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

            InfoLineEdit {
                id: lineEditSIPCertPassword

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                echoMode: TextInput.Password

                Layout.row: 7
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.lineEditSIPCertPasswordLineEditTextChanged(text)
                }
            }

            // nineth row
            ToggleSwitch{
                id: verifyIncomingCertificatesServerToogle

                labelText: qsTr("Verify Certificates(Server Side)")
                fontPointSize: 10

                Layout.row: 8
                Layout.column: 0

                onSwitchToggled: {
                    SettingsAdaptor.setVerifyCertificatesServer(checked)
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setVerifyCertificatesClient(checked)
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setRequireCertificatesIncomingTLS(checked)
                }
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

            SettingParaCombobox{
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

                model: ListModel{
                    ListElement{textDisplay: "Default"; firstArg: "Default"; secondArg: 0}
                    ListElement{textDisplay: "TLSv1"; firstArg: "TLSv1"; secondArg: 1}
                    ListElement{textDisplay: "TLSv1.1"; firstArg: "TLSv1.1"; secondArg: 2}
                    ListElement{textDisplay: "TLSv1.2"; firstArg: "TLSv1.2"; secondArg: 3}
                }

                onActivated: {
                    var indexOfOption = tlsProtocolComboBox.get(index).secondArg
                    SettingsAdaptor.tlsProtocolComboBoxIndexChanged(parseInt(indexOfOption))
                }
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

            InfoLineEdit {
                id: outgoingTLSServerNameLineEdit

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 12
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.outgoingTLSServerNameLineEditTextChanged(text)
                }
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

                onValueModified: {
                    SettingsAdaptor.negotiationTimeoutSpinBoxValueChanged(value)
                }
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

                    onValueModified: {
                        SettingsAdaptor.registrationTimeoutSpinBoxValueChanged(value)
                    }
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

                    onValueModified: {
                        SettingsAdaptor.networkInterfaceSpinBoxValueChanged(value)
                    }
                }

            // 3rd row
            ToggleSwitch{
                id: checkBoxUPnPSIP

                labelText: qsTr("Use UPnP")
                fontPointSize: 10

                Layout.row: 2
                Layout.column: 0

                onSwitchToggled: {
                    SettingsAdaptor.setUseUPnP(checked)
                }
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

                onSwitchToggled: {
                    SettingsAdaptor.setUseTURN(checked)
                    lineEditTurnAddressSIP.enabled = checked
                    lineEditTurnUsernameSIP.enabled = checked
                    lineEditTurnPsswdSIP.enabled = checked
                    lineEditTurnRealmSIP.enabled = checked
                }
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


            InfoLineEdit {
                id: lineEditTurnAddressSIP

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 4
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.setTURNAddress(text)
                }
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

            InfoLineEdit {
                id: lineEditTurnUsernameSIP

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 5
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.setTURNUsername(text)
                }
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

            InfoLineEdit {
                id: lineEditTurnPsswdSIP

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 6
                Layout.column: 2

                echoMode: TextInput.Password

                onEditingFinished: {
                    SettingsAdaptor.setTURNPassword(text)
                }
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

            InfoLineEdit {
                id: lineEditTurnRealmSIP

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 7
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.setTURNRealm(text)
                }
            }

            // 9th row
            ToggleSwitch{
                id: checkBoxSTUNEnableSIP

                labelText: qsTr("Use STUN")
                fontPointSize: 10

                Layout.row: 8
                Layout.column: 0

                onSwitchToggled: {
                    SettingsAdaptor.setUseSTUN(checked)
                    lineEditSTUNAddressSIP.enabled = checked
                }
            }

            Item{
                Layout.fillHeight: true

                Layout.maximumWidth: 20
                Layout.preferredWidth: 20
                Layout.minimumWidth: 20

                Layout.row: 8
                Layout.column: 1
            }

            InfoLineEdit {
                id: lineEditSTUNAddressSIP

                Layout.alignment: Qt.AlignCenter

                fieldLayoutWidth: 250
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                Layout.row: 8
                Layout.column: 2

                onEditingFinished: {
                    SettingsAdaptor.setSTUNAddress(text)
                }
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

                    onSwitchToggled: {
                        SettingsAdaptor.setUseCustomAddressAndPort(checked)
                        lineEditSIPCustomAddress.enabled = checked
                        customPortSIPSpinBox.enabled = checked
                    }
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

                InfoLineEdit {
                    id: lineEditSIPCustomAddress

                    Layout.alignment: Qt.AlignCenter

                    fieldLayoutWidth: 250
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 1
                    Layout.column: 2

                    onEditingFinished: {
                        SettingsAdaptor.lineEditSIPCustomAddressLineEditTextChanged(text)
                    }
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

                    onValueModified: {
                        SettingsAdaptor.customPortSIPSpinBoxValueChanged(value)
                    }
                }
            }
        }

        // media section
        ColumnLayout{
            spacing: 6
            Layout.fillWidth: true

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 27
                Layout.preferredHeight: 27
                Layout.maximumHeight: 27

                text: qsTr("Media")

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
                spacing: 6
                Layout.fillWidth: true

                ToggleSwitch {
                    id: videoCheckBoxSIP

                    Layout.leftMargin: 20

                    labelText: qsTr("Enable Video")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setVideoState(checked)
                    }
                }

                RowLayout {
                    spacing: 6
                    Layout.fillWidth: true
                    Layout.leftMargin: 20

                    ColumnLayout {
                        spacing: 6
                        Layout.maximumWidth: 348

                        RowLayout {
                            spacing: 6
                            Layout.fillWidth: true

                            Layout.maximumHeight: 30

                            Label {
                                Layout.fillWidth: true

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Video Codecs")
                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true

                                Layout.minimumWidth: 20
                                Layout.preferredWidth: 20
                                Layout.maximumWidth: 20
                            }

                            HoverableRadiusButton {
                                id: videoDownPushButtonSIP

                                Layout.minimumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.maximumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2
                                scale: 1

                                buttonImageHeight: height
                                buttonImageWidth: height

                                font.pointSize: 9
                                font.kerning: true

                                icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                icon.width: 32
                                icon.height: 32

                                onClicked: {
                                    decreaseVideoCodecPriority()
                                }
                            }

                            HoverableRadiusButton {
                                id: videoUpPushButtonSIP

                                Layout.minimumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.maximumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2

                                buttonImageHeight: height
                                buttonImageWidth: height

                                font.pointSize: 9
                                font.kerning: true

                                icon.source: "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                                icon.width: 32
                                icon.height: 32

                                onClicked: {
                                    increaseVideoCodecPriority()
                                }
                            }
                        }

                        ListViewJami {
                            id: videoListWidgetSIP

                            Layout.minimumWidth: 348
                            Layout.preferredWidth: 348
                            Layout.maximumWidth: 348

                            Layout.minimumHeight: 192
                            Layout.preferredHeight: 192
                            Layout.maximumHeight: 192

                            model: videoCodecListModelSIP

                            delegate: VideoCodecDelegate {
                                id: videoCodecDelegate

                                width: videoListWidgetSIP.width
                                height: videoListWidgetSIP.height / 4

                                videoCodecName : VideoCodecName
                                isEnabled : IsEnabled
                                videoCodecId: VideoCodecID

                                onClicked: {
                                    videoListWidgetSIP.currentIndex = index
                                }

                                onVideoCodecStateChange:{
                                    SettingsAdaptor.videoCodecsStateChange(idToSet , isToBeEnabled)
                                    updateVideoCodecs()
                                }
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 6
                        Layout.maximumWidth: 348

                        RowLayout {
                            spacing: 6
                            Layout.fillWidth: true

                            Layout.maximumHeight: 30

                            Label {
                                Layout.fillWidth: true

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Audio Codecs")
                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true

                                Layout.minimumWidth: 20
                                Layout.preferredWidth: 20
                                Layout.maximumWidth: 20
                            }

                            HoverableRadiusButton {
                                id: audioDownPushButtonSIP

                                Layout.minimumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.maximumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2

                                buttonImageHeight: height
                                buttonImageWidth: height

                                font.pointSize: 9
                                font.kerning: true

                                icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                icon.width: 32
                                icon.height: 32

                                onClicked: {
                                    decreaseAudioCodecPriority()
                                }
                            }

                            HoverableRadiusButton {
                                id: audioUpPushButtonSIP

                                Layout.minimumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.maximumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2

                                buttonImageHeight: height
                                buttonImageWidth: height

                                font.pointSize: 9
                                font.kerning: true

                                icon.source: "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                                icon.width: 32
                                icon.height: 32

                                onClicked: {
                                    increaseAudioCodecPriority()
                                }
                            }
                        }

                        ListViewJami {
                            id: audioListWidgetSIP

                            Layout.minimumWidth: 348
                            Layout.preferredWidth: 348
                            Layout.maximumWidth: 348

                            Layout.minimumHeight: 192
                            Layout.preferredHeight: 192
                            Layout.maximumHeight: 192

                            model: audioCodecListModelSIP

                            delegate: AudioCodecDelegate {
                                id: audioCodecDelegate

                                width: audioListWidgetSIP.width
                                height: audioListWidgetSIP.height / 4

                                layer.mipmap: false
                                clip: true

                                audioCodecName : AudioCodecName
                                isEnabled : IsEnabled
                                audioCodecId: AudioCodecID
                                samplerRate: Samplerate

                                onClicked: {
                                    audioListWidgetSIP.currentIndex = index
                                }

                                onAudioCodecStateChange:{
                                    SettingsAdaptor.audioCodecsStateChange(idToSet , isToBeEnabled)
                                    updateAudioCodecs()
                                }
                            }
                        }
                    }
                }
            }

        }

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 20
            Layout.preferredHeight: 20
            Layout.minimumHeight: 20
        }

        ColumnLayout{
            spacing: 6
            Layout.fillWidth: true

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 27
                Layout.preferredHeight: 27
                Layout.maximumHeight: 27

                text: qsTr("SDP Session Negotiation(ICE Fallback)")

                font.pointSize: 13
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Label {
                Layout.fillWidth: true

                Layout.minimumHeight: 21
                Layout.preferredHeight: 21
                Layout.maximumHeight: 21

                text: qsTr("Only used during negotiation in case ICE is not supported")

                font.pointSize: 10
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

                rows: 4
                columns: 3

                // 1st row
                Label{
                    Layout.minimumWidth: 162
                    Layout.preferredWidth: 162
                    Layout.maximumWidth: 162

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Audio RTP Min Port")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 0
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.row: 0
                    Layout.column: 1
                }

                SpinBox{
                    id:audioRTPMinPortSpinBox

                    Layout.minimumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 250

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65535
                    stepSize: 1

                    Layout.row: 0
                    Layout.column: 2

                    onValueModified: {
                        audioRTPMinPortSpinBoxEditFinished(value)
                    }
                }

                // 2nd row
                Label{
                    Layout.minimumWidth: 162
                    Layout.preferredWidth: 162
                    Layout.maximumWidth: 162

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Audio RTP Max Port")
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

                SpinBox{
                    id:audioRTPMaxPortSpinBox

                    Layout.minimumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 250

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65535
                    stepSize: 1

                    Layout.row: 1
                    Layout.column: 2

                    onValueModified: {
                        audioRTPMaxPortSpinBoxEditFinished(value)
                    }
                }

                // 3rd row
                Label{
                    Layout.minimumWidth: 162
                    Layout.preferredWidth: 162
                    Layout.maximumWidth: 162

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Video RTP Min Port")
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
                    id:videoRTPMinPortSpinBox

                    Layout.minimumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 250

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65535
                    stepSize: 1

                    Layout.row: 2
                    Layout.column: 2

                    onValueModified: {
                        videoRTPMinPortSpinBoxEditFinished(value)
                    }
                }

                // 4th row
                Label{
                    Layout.minimumWidth: 162
                    Layout.preferredWidth: 162
                    Layout.maximumWidth: 162

                    Layout.minimumHeight: 28
                    Layout.preferredHeight: 28
                    Layout.maximumHeight: 28

                    text: qsTr("Video RTP Max Port")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    Layout.row: 3
                    Layout.column: 0
                }

                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.row: 3
                    Layout.column: 1
                }

                SpinBox{
                    id:videoRTPMaxPortSpinBox

                    Layout.minimumWidth: 250
                    Layout.preferredWidth: 250
                    Layout.maximumWidth: 250

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    font.pointSize: 10
                    font.kerning: true

                    from: 0
                    to: 65535
                    stepSize: 1

                    Layout.row: 3
                    Layout.column: 2

                    onValueModified: {
                        videoRTPMaxPortSpinBoxEditFinished(value)
                    }
                }
            }
        }

        // spacers
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


