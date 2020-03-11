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

import QtQuick 2.14
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
    function updateAccountInfoDisplayedAdvance() {
        //Call Settings
        checkAutoConnectOnLocalNetwork.checked = SettingsAdaptor.getAccountConfig_PeerDiscovery()
        checkBoxUntrusted.checked = SettingsAdaptor.getAccountConfig_DHT_PublicInCalls()
        checkBoxAutoAnswer.checked = SettingsAdaptor.getAccountConfig_AutoAnswer()
        checkBoxCustomRingtone.checked = SettingsAdaptor.getAccountConfig_Ringtone_RingtoneEnabled()

        // Name Server
        lineEditNameServer.text = SettingsAdaptor.getAccountConfig_RingNS_Uri()

        //OpenDHT Config
        checkBoxEnableProxy.checked = SettingsAdaptor.getAccountConfig_ProxyEnabled()
        lineEditProxy.text = SettingsAdaptor.getAccountConfig_ProxyServer()
        lineEditBootstrap.text = SettingsAdaptor.getAccountConfig_Hostname()

        // Security
        btnCACert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateListFile())
        btnUserCert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateFile())
        btnPrivateKey.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_PrivateKeyFile())

        // Connectivity
        checkBoxUPnP.checked = SettingsAdaptor.getAccountConfig_UpnpEnabled()
        checkBoxTurnEnable.checked = SettingsAdaptor.getAccountConfig_TURN_Enabled()
        lineEditTurnAddress.text = SettingsAdaptor.getAccountConfig_TURN_Server()
        lineEditTurnUsername.text = SettingsAdaptor.getAccountConfig_TURN_Username()
        lineEditTurnPassword.text = SettingsAdaptor.getAccountConfig_TURN_Password()
        checkBoxSTUNEnable.checked = SettingsAdaptor.getAccountConfig_STUN_Enabled()
        lineEditSTUNAddress.text = SettingsAdaptor.getAccountConfig_STUN_Server()
        // codecs
        videoCheckBox.checked = SettingsAdaptor.getAccountConfig_Video_Enabled()
            // update audio and video codec, make sure this change does not trigger item change events
        updateAudioCodecs();
        updateVideoCodecs();
        btnRingtone.enabled = SettingsAdaptor.getAccountConfig_Ringtone_RingtoneEnabled()
        btnRingtone.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath())
        lineEditProxy.enabled = SettingsAdaptor.getAccountConfig_ProxyEnabled()
        lineEditSTUNAddress.enabled = SettingsAdaptor.getAccountConfig_STUN_Enabled()
    }

    function updateAudioCodecs(){
        audioCodecListModel.layoutAboutToBeChanged()
        audioCodecListModel.dataChanged(audioCodecListModel.index(0, 0),
                                     audioCodecListModel.index(audioCodecListModel.rowCount() - 1, 0))
        audioCodecListModel.layoutChanged()
    }

    function updateVideoCodecs(){
        videoCodecListModel.layoutAboutToBeChanged()
        videoCodecListModel.dataChanged(videoCodecListModel.index(0, 0),
                                     videoCodecListModel.index(videoCodecListModel.rowCount() - 1, 0))
        videoCodecListModel.layoutChanged()
    }

    function decreaseAudioCodecPriority(){
        var index = audioListWidget.currentIndex
        var codecId = audioCodecListModel.data(audioCodecListModel.index(index,0), AudioCodecListModel.AudioCodecID)

        SettingsAdaptor.decreaseAudioCodecPriority(codecId)
        audioListWidget.currentIndex = index + 1
        updateAudioCodecs()
    }

    function increaseAudioCodecPriority(){
        var index = audioListWidget.currentIndex
        var codecId = audioCodecListModel.data(audioCodecListModel.index(index,0), AudioCodecListModel.AudioCodecID)

        SettingsAdaptor.increaseAudioCodecPriority(codecId)
        audioListWidget.currentIndex = index - 1
        updateAudioCodecs()
    }

    function decreaseVideoCodecPriority(){
        var index = videoListWidget.currentIndex
        var codecId = videoCodecListModel.data(videoCodecListModel.index(index,0), VideoCodecListModel.VideoCodecID)

        SettingsAdaptor.decreaseVideoCodecPriority(codecId)
        videoListWidget.currentIndex = index + 1
        updateVideoCodecs()
    }

    function increaseVideoCodecPriority(){
        var index = videoListWidget.currentIndex
        var codecId = videoCodecListModel.data(videoCodecListModel.index(index,0), VideoCodecListModel.VideoCodecID)

        SettingsAdaptor.increaseVideoCodecPriority(codecId)
        videoListWidget.currentIndex = index - 1
        updateVideoCodecs()
    }

    VideoCodecListModel{
        id: videoCodecListModel
    }

    AudioCodecListModel{
        id: audioCodecListModel
    }

    function changeRingtonePath(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_RingtonePath(url)
            btnRingtone.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath())
        } else if (SettingsAdaptor.getAccountConfig_Ringtone_RingtonePath().length === 0){
            btnRingtone.text = qsTr("Add a custom ringtone")
        }
    }

    function changeFileCACert(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FileCACert(url)
            btnCACert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateListFile())
        }
    }

    function changeFileUserCert(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FileUserCert(url)
            btnUserCert.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_CertificateFile())
        }
    }

    function changeFilePrivateKey(url){
        if(url.length !== 0) {
            SettingsAdaptor.set_FilePrivateKey(url)
            btnPrivateKey.text = UtilsAdapter.toFileInfoName(SettingsAdaptor.getAccountConfig_TLS_PrivateKeyFile())
        }
    }

    FileDialog {
        id: ringtonePath_Dialog

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
        id: caCert_Dialog

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
        id: userCert_Dialog

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
        id: privateKey_Dialog

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

                Layout.leftMargin: 20

                labelText: qsTr("Allow Calls From Unstructed Peers")
                fontPointSize: 10

                onSwitchToggled: {
                    SettingsAdaptor.setCallsUntrusted(checked)
                }
            }

            ToggleSwitch {
                id: checkBoxAutoAnswer

                Layout.fillWidth: true
                Layout.leftMargin: 20

                labelText: qsTr("Auto Answer Calls")
                fontPointSize: 10

                onSwitchToggled: {
                    SettingsAdaptor.setAutoAnswerCalls(checked)
                }
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

                    onSwitchToggled: {
                        SettingsAdaptor.setEnableRingtone(checked)
                        btnRingtone.enabled = checked
                    }
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

                    onClicked: {
                        ringtonePath_Dialog.open()
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

            Label {
                Layout.minimumWidth: 60

                Layout.minimumHeight: 29
                Layout.preferredHeight: 29
                Layout.maximumHeight: 29

                text: qsTr("Address")

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
                id: lineEditNameServer

                fieldLayoutWidth: 300
                fieldLayoutHeight: 29

                font.pointSize: 10
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter

                onEditingFinished: {
                    SettingsAdaptor.setNameServer(text)
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

            text: qsTr("OpenDHT Configuration")
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

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20

                ToggleSwitch {
                    id: checkBoxEnableProxy

                    labelText: qsTr("Enable proxy")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setEnableProxy(checked)
                        lineEditProxy.enabled = checked
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                InfoLineEdit {
                    id: lineEditProxy

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    onEditingFinished: {
                        SettingsAdaptor.setProxyAddress(text)
                    }
                }
            }

            RowLayout {
                spacing: 6
                Layout.fillWidth: true
                Layout.leftMargin: 20

                Label {
                    id: labelBootstrap

                    Layout.minimumWidth: 72
                    Layout.preferredWidth: 72

                    Layout.minimumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.maximumHeight: 29

                    text: qsTr("Bootstrap")
                    font.pointSize: 10
                    font.kerning: true
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                }

                InfoLineEdit {
                    id: lineEditBootstrap

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    onEditingFinished: {
                        SettingsAdaptor.setBootstrapAddress(text)
                    }
                }
            }
        }
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

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            GridLayout {
                rows: 4
                columns: 2
                rowSpacing: 0
                columnSpacing: 6

                Layout.fillWidth: true
                Layout.leftMargin: 20

                // CA Certificate
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("CA Certificate")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnCACert

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16

                    onClicked: {
                        caCert_Dialog.open()
                    }
                }

                // User Certificate
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("User Certificate")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnUserCert

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16

                    onClicked: {
                        userCert_Dialog.open()
                    }
                }

                // Private Key
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 32
                    Layout.preferredHeight: 32
                    Layout.maximumHeight: 32

                    text: qsTr("Private Key")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                HoverableButton {
                    id: btnPrivateKey

                    radius: height / 2

                    Layout.minimumWidth: 298
                    Layout.preferredWidth: 298
                    Layout.maximumWidth: 298

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    icon.source: "qrc:/images/icons/round-folder-24px.svg"
                    icon.width: 16
                    icon.height: 16

                    onClicked: {
                        privateKey_Dialog.open()
                    }
                }

                // Private key password
                Label {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 29
                    Layout.preferredHeight: 29
                    Layout.maximumHeight: 29

                    text: qsTr("Private Key Password")
                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditCertPassword

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    echoMode: TextInput.Password
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
            Layout.topMargin: 10

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

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            GridLayout {
                Layout.leftMargin: 20
                Layout.fillWidth: true

                rows: 6
                columns: 3
                rowSpacing: 6
                columnSpacing: 6

                // row 2
                ToggleSwitch {
                    id: checkAutoConnectOnLocalNetwork

                    Layout.row: 0
                    Layout.column: 0

                    labelText: qsTr("Auto Connect On Local Network")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setAutoConnectOnLocalNetwork(checked)
                    }
                }

                Item {
                    Layout.row: 0
                    Layout.column: 1

                    Layout.fillHeight: true

                    Layout.minimumWidth: 40
                    Layout.preferredWidth: 40
                    Layout.maximumWidth: 40
                }

                // row 2
                ToggleSwitch {
                    id: checkBoxUPnP

                    Layout.row: 1
                    Layout.column: 0

                    labelText: qsTr("Use UPnP")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setUseUPnP(checked)
                    }
                }

                Item {
                    Layout.row: 1
                    Layout.column: 1

                    Layout.fillHeight: true

                    Layout.minimumWidth: 40
                    Layout.preferredWidth: 40
                    Layout.maximumWidth: 40
                }

                // row 3
                ToggleSwitch {
                    id: checkBoxTurnEnable

                    Layout.row: 2
                    Layout.column: 0

                    labelText: qsTr("Use TURN")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setUseTURN(checked)
                    }
                }

                // row 4
                Label {
                    Layout.row: 3
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Address")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnAddress

                    Layout.row: 3
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    onEditingFinished: {
                        SettingsAdaptor.setTURNAddress(text)
                    }
                }

                //row 5
                Label {
                    Layout.row: 4
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Username")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnUsername

                    Layout.row: 4
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    onEditingFinished: {
                        SettingsAdaptor.setTURNUsername(text)
                    }
                }

                //row 6
                Label {
                    Layout.row: 5
                    Layout.column: 0

                    Layout.minimumWidth: 124

                    Layout.minimumHeight: 27
                    Layout.preferredHeight: 27
                    Layout.maximumHeight: 27

                    text: qsTr("TURN Password")

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                InfoLineEdit {
                    id: lineEditTurnPassword
                    layer.mipmap: false

                    Layout.row: 5
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    echoMode: TextInput.Password

                    onEditingFinished: {
                        SettingsAdaptor.setTURNPassword(text)
                    }
                }

                // row 7
                ToggleSwitch {
                    id: checkBoxSTUNEnable

                    Layout.row: 6
                    Layout.column: 0

                    labelText: qsTr("Use STUN")
                    fontPointSize: 10

                    onSwitchToggled: {
                        SettingsAdaptor.setUseSTUN(checked)
                        lineEditSTUNAddress.enabled = checked
                    }
                }

                InfoLineEdit {
                    id: lineEditSTUNAddress

                    Layout.row: 6
                    Layout.column: 2

                    fieldLayoutWidth: 300
                    fieldLayoutHeight: 29

                    font.pointSize: 10
                    font.kerning: true

                    placeholderText: qsTr("STUN Address")

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    onEditingFinished: {
                        SettingsAdaptor.setSTUNAddress(text)
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

    ColumnLayout {
        spacing: 6
        Layout.fillWidth: true

        Label {
            Layout.fillWidth: true
            Layout.topMargin: 10

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

            Layout.minimumHeight: 10
            Layout.preferredHeight: 10
            Layout.maximumHeight: 10
        }

        ColumnLayout {
            spacing: 6
            Layout.fillWidth: true

            ToggleSwitch {
                id: videoCheckBox

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
                    //Layout.fillWidth: true
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

                        HoverableButton {
                            id: videoDownPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2
                            scale: 1

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                            icon.width: 32
                            icon.height: 32

                            onClicked: {
                                decreaseVideoCodecPriority()
                            }
                        }

                        HoverableButton {
                            id: videoUpPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

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
                        id: videoListWidget

                        Layout.minimumWidth: 348
                        Layout.preferredWidth: 348
                        Layout.maximumWidth: 348

                        Layout.minimumHeight: 192
                        Layout.preferredHeight: 192
                        Layout.maximumHeight: 192

                        model: videoCodecListModel

                        delegate: VideoCodecDelegate {
                            id: videoCodecDelegate

                            width: videoListWidget.width
                            height: videoListWidget.height / 4

                            videoCodecName : VideoCodecName
                            isEnabled : IsEnabled
                            videoCodecId: VideoCodecID

                            onClicked: {
                                videoListWidget.currentIndex = index
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

                        HoverableButton {
                            id: audioDownPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            font.pointSize: 9
                            font.kerning: true

                            icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                            icon.width: 32
                            icon.height: 32

                            onClicked: {
                                decreaseAudioCodecPriority()
                            }
                        }

                        HoverableButton {
                            id: audioUpPushButton

                            Layout.minimumWidth: 30
                            Layout.preferredWidth: 30
                            Layout.maximumWidth: 30

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

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
                        id: audioListWidget

                        Layout.minimumWidth: 348
                        Layout.preferredWidth: 348
                        Layout.maximumWidth: 348

                        Layout.minimumHeight: 192
                        Layout.preferredHeight: 192
                        Layout.maximumHeight: 192

                        model: audioCodecListModel

                        delegate: AudioCodecDelegate {
                            id: audioCodecDelegate

                            width: audioListWidget.width
                            height: audioListWidget.height / 4

                            layer.mipmap: false
                            clip: true

                            audioCodecName : AudioCodecName
                            isEnabled : IsEnabled
                            audioCodecId: AudioCodecID
                            samplerRate: Samplerate

                            onClicked: {
                                audioListWidget.currentIndex = index
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

        Layout.minimumHeight: 48
        Layout.preferredHeight: 48
        Layout.maximumHeight: 48
    }
}
