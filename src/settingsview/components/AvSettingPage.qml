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
import net.jami.Models 1.0

Rectangle {
    id: avSettingPage

    function populateAVSettings(){
        inputComboBox.clear()
        // get the last index of inputDevices
        var inputDevices = ClientWrapper.avmodel.getAudioInputDevices()
        var inputIndex = inputDevices.length - 1
        for(var id of inputDevices){
            inputComboBox.append({"textDisplay": ClientWrapper.utilsAdaptor.getStringUTF8(id), "firstArg": ClientWrapper.utilsAdaptor.getStringUTF8(id), "secondArg": 0})
        }
        inputComboBox.currentIndex = (inputIndex !== -1 ? inputIndex : 0)

        outputComboBox.clear()
        var outputDevices = ClientWrapper.avmodel.getAudioOutputDevices()
        var outputIndex = outputDevices.length -1
        for (var od of outputDevices) {
            outputComboBox.append({"textDisplay": ClientWrapper.utilsAdaptor.getStringUTF8(od), "firstArg": ClientWrapper.utilsAdaptor.getStringUTF8(od), "secondArg": 0})
        }
        outputComboBox.currentIndex = (outputIndex !== -1 ? outputIndex : 0)

        populateVideoSettings()
        var encodeAccel = ClientWrapper.avmodel.getHardwareAcceleration()
        hardwareAccelControl.checked = encodeAccel
    }

    function populateVideoSettings() {
        deviceBox.clear()
        formatBox.clear()

        deviceBox.enabled = (ClientWrapper.avmodel.getDevices().length > 0)
        formatBox.enabled = (ClientWrapper.avmodel.getDevices().length > 0)
        labelVideoDevice.enabled = (ClientWrapper.avmodel.getDevices().length > 0)
        labelVideoFormat.enabled = (ClientWrapper.avmodel.getDevices().length > 0)

        if(ClientWrapper.avmodel.getDevices().length === 0){
            deviceBox.append({"textDisplay": qsTr("No Device"), "firstArg": "", "secondArg": 0})
            deviceBox.currentIndex = 0

            formatBox.append({"textDisplay": qsTr("No Device"), "firstArg": "", "secondArg": 0})
            formatBox.currentIndex = 0
        } else {
            var currentCaptureDevice = ClientWrapper.avmodel.getCurrentVideoCaptureDevice()
            // get last index of device vector
            var deviceLastIndex = ClientWrapper.avmodel.getDevices().length - 1
            var devices = ClientWrapper.avmodel.getDevices()
            try{
                for(var d of devices){
                    deviceBox.append({"textDisplay": ClientWrapper.utilsAdaptor.getStringUTF8(ClientWrapper.settingsAdaptor.get_Video_Settings_Name(d)), "firstArg": ClientWrapper.utilsAdaptor.getStringUTF8(ClientWrapper.settingsAdaptor.get_Video_Settings_Name(d)), "secondArg": ""})
                }
            } catch(err) {}

            deviceBox.currentIndex = deviceLastIndex
            slotDeviceBoxCurrentIndexChanged(deviceBox.currentIndex)
            // set format list
            setFormatListForDevice(ClientWrapper.avmodel.getCurrentVideoCaptureDevice())

            try{
                startPreviewing(false)
            } catch (err2){}
        }
    }

    function setFormatListForDevice(device){
        if(ClientWrapper.settingsAdaptor.get_DeviceCapabilitiesSize(device) === 0) return

        try{
            formatBox.clear()

            var resolutions = ClientWrapper.settingsAdaptor.getResolutions(device)
            var rates = ClientWrapper.settingsAdaptor.getFrameRates(device)

            for(var i=0; i < resolutions.length; i++){
                var resolution = resolutions[i]
                var rate = rates[i]

                formatBox.append({"textDisplay": resolution +" [" + JSON.stringify(rate) +" fps]", "firstArg": resolution, "secondArg": rate})
            }

            formatBox.currentIndex = resolutions.length - 1
            slotFormatBoxCurrentIndexChanged(formatBox.currentIndex)
        } catch(err){console.warn("Exception: " + err.message)}
    }

    function startPreviewing(force = false){
        ClientWrapper.accountAdaptor.startPreviewing(force)
        previewAvailable = true
    }

    function stopPreviewing(){
        ClientWrapper.accountAdaptor.stopPreviewing()
    }

    function startAudioMeter(async = true){
        audioInputMeter.start()
        ClientWrapper.accountAdaptor.startAudioMeter(async)
    }

    function stopAudioMeter(async = true){
        audioInputMeter.stop()
        ClientWrapper.accountAdaptor.stopAudioMeter(async)
    }

    // slots for av page
    function slotAudioMeter(id, level){
        if (id === "audiolayer_id") {
                audioInputMeter.setLevel(level)
            }
    }

    function slotSetHardwareAccel(state){
        ClientWrapper.accountAdaptor.avModel().setHardwareAcceleration(state)
        startPreviewing(true)
    }

    function slotAudioOutputIndexChanged(index){
        stopAudioMeter(false)
        var selectedOutputDeviceName = outputComboBox.get(index).firstArg
        ClientWrapper.avmodel.setOutputDevice(selectedOutputDeviceName)
        startAudioMeter(false)
    }

    function slotAudioInputIndexChanged(index){
        stopAudioMeter(false)
        var selectedInputDeviceName = inputComboBox.get(index).firstArg
        ClientWrapper.avmodel.setInputDevice(selectedInputDeviceName)
        startAudioMeter(false)
    }

    function slotDeviceBoxCurrentIndexChanged(index){
        var deviceName = deviceBox.get(index).firstArg
        var devices = ClientWrapper.avmodel.getDevices()
        try{
            var deviceId = ClientWrapper.avmodel.getDeviceIdFromName(deviceName)
            if(deviceId.length === 0){
                console.warn("Couldn't find device: " + deviceName)
                return
            }

            ClientWrapper.avmodel.setCurrentVideoCaptureDevice(deviceId)
            ClientWrapper.avmodel.setDefaultDevice(deviceId)
            setFormatListForDevice(deviceId)
            startPreviewing(true)
        } catch(err){console.warn(err.message)}
    }

    function slotFormatBoxCurrentIndexChanged(index){
        var resolution = formatBox.get(index).firstArg
        var rate = formatBox.get(index).secondArg
        var device = ClientWrapper.avmodel.getCurrentVideoCaptureDevice()
        try{
            ClientWrapper.settingsAdaptor.set_Video_Settings_Rate_And_Resolution(device,rate,resolution)
        } catch(error){console.warn(error.message)}
    }

    function slotVideoDeviceListChanged(){
        populateVideoSettings()
    }

    //property AVModel avmodel: ClientWrapper.accountAdaptor.avModel()
    //property RenderManager renderer: ClientWrapper.accountAdaptor.getRenderManager()

//    Component.onDestruction: {
//        ClientWrapper.avmodel = null
//        renderer = null
//    }

//    Component.onCompleted: {
//        ClientWrapper.avmodel = ClientWrapper.accountAdaptor.avModel()
//        renderer= ClientWrapper.accountAdaptor.getRenderManager()
//    }

    property bool previewAvailable: false

    Connections{
        target: ClientWrapper.avmodel

        function onAudioMeter(id, level){
            slotAudioMeter(id,level)
        }
    }

    Connections{
        target: ClientWrapper.renderManager

        function onVideoDeviceListChanged(){
            slotVideoDeviceListChanged()
        }
    }

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

                        SettingParaCombobox {
                            id: inputComboBox

                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true

                            onActivated: {
                                slotAudioInputIndexChanged(index)
                            }
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

                        SettingParaCombobox {
                            id: outputComboBox

                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true

                            onActivated: {
                                slotAudioOutputIndexChanged(index)
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
                            id: labelVideoDevice

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

                        SettingParaCombobox {
                            id: deviceBox

                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true

                            onActivated: {
                                slotDeviceBoxCurrentIndexChanged(index)
                            }
                        }
                    }

                    RowLayout {
                        spacing: 7
                        Layout.fillWidth: true
                        Layout.leftMargin: 20
                        Layout.maximumHeight: 30

                        Label {
                            id: labelVideoFormat

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

                        SettingParaCombobox {
                            id: formatBox

                            Layout.maximumWidth: 360
                            Layout.preferredWidth: 360
                            Layout.minimumWidth: 360

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            font.pointSize: 10
                            font.kerning: true

                            onActivated: {
                                slotFormatBoxCurrentIndexChanged(index)
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

                onSwitchToggled: {
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
