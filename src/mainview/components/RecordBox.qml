/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Albert Babí <albert.babi@savoirfairelinux.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.0
import net.jami.Models 1.0
import "../../commoncomponents"


Rectangle {

    enum States {
        INIT,
        RECORDING,
        REC_SUCCESS
    }

    id:recBox
    color: "#FFFFFF"
    width: 320
    height: 240
    radius: 5
    border.color: JamiTheme.tabbarBorderColor

    property string pathRecorder: ""
    property string timeText: "00:00"
    property int duration: 0
    property int state: RecordBox.States.INIT
    property bool isVideo: false
    property bool previewAvailable: false

    property int h_offset: -65
    property int v_offset: -65

    function openRecorder(set_x, set_y, vid) {
        focus = true
        visible = true
        x = set_x+(width/2)+h_offset
        y = set_y-(height/2)+v_offset

        updateState(RecordBox.States.INIT)

        isVideo = vid
        if (isVideo){
            ClientWrapper.accountAdaptor.startPreviewing(false)
            previewAvailable = true
        }
    }

    onActiveFocusChanged:  {
        if (visible)
            closeRecorder()
    }

    function closeRecorder() {
        if (isVideo && ClientWrapper.accountAdaptor.isPreviewing())
            ClientWrapper.accountAdaptor.stopPreviewing()
        stopRecording()
        visible = false
    }

    function updateState(new_state) {
        state = new_state
        recordButton.visible = (state == RecordBox.States.INIT)
        btnStop.visible = (state == RecordBox.States.RECORDING)
        btnRestart.visible = (state == RecordBox.States.REC_SUCCESS)
        btnSend.visible = (state == RecordBox.States.REC_SUCCESS)

        if (state == RecordBox.States.INIT) {
            duration = 0
            time.text = "00:00"
            timer.stop()
        } else if (state == RecordBox.States.REC_SUCCESS) {
            timer.stop()
        }
    }

    function startRecording() {
        timer.start()
        pathRecorder = ClientWrapper.avmodel.startLocalRecorder(!isVideo)
        if (pathRecorder == "") {
            timer.stop()
        }
    }

    function stopRecording(){
        if (pathRecorder !=  ""){
            ClientWrapper.avmodel.stopLocalRecorder(pathRecorder)
        }
    }

    function sendRecord(){
        if (pathRecorder != "")
            MessagesAdapter.sendFile(pathRecorder)
    }

    function updateTimer() {

        duration += 1

        var m = Math.trunc(duration / 60)
        var s = (duration % 60)

        var min = (m < 10) ? "0" + String(m) : String(m)
        var sec = (s < 10) ? "0" + String(s) : String(s)

        time.text = min + ":" + sec;
    }


    Connections{
        target: ClientWrapper.renderManager
    }

    Rectangle {
        visible: (isVideo && previewAvailable)
        Layout.alignment: Qt.AlignHCenter

        anchors.fill: parent
        color: "black"
        radius: 5

        PreviewRenderer{
            id: previewWidget
            Layout.alignment: Qt.AlignHCenter

            height: parent.height-3
            width: parent.width-3
            x: (parent.width - width)/2
            y: 3
        }
    }

    Label {
        visible: (isVideo && !previewAvailable)

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignCenter

        text: qsTr("Preview unavailable")
        font.pointSize: 10
        font.kerning: true
    }

    Timer {
        id: timer
        interval: 1000;
        running: false;
        repeat: true
        onTriggered: updateTimer()
    }

    Text {
        id: time
        visible: true
        text: "00:00"
        color: (isVideo ? "white" : "black")
        font.pointSize: (isVideo ? 12 : 20)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: (isVideo ? 100 : 0)
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: (isVideo ? 100 : 0)
    }


    HoverableRadiusButton {
        id: recordButton
        Layout.maximumWidth: 30
        Layout.preferredWidth: 30
        Layout.minimumWidth: 30

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        buttonImageHeight: height
        buttonImageWidth: height
        backgroundColor: "white"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        radius: height / 2

        icon.source: "qrc:/images/icons/av_icons/fiber_manual_record-24px.svg"
        icon.height: 24
        icon.width: 24
        onClicked: {
            updateState(RecordBox.States.RECORDING)
            startRecording()
        }
    }

    HoverableRadiusButton {
        id: btnStop
        Layout.maximumWidth: 30
        Layout.preferredWidth: 30
        Layout.minimumWidth: 30

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        buttonImageHeight: height
        buttonImageWidth: height
        backgroundColor: "white"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        radius: height / 2

        icon.source: "qrc:/images/icons/av_icons/stop-24px-red.svg"
        icon.height: 24
        icon.width: 24
        onClicked: {
            stopRecording()
            updateState(RecordBox.States.REC_SUCCESS)
        }
    }

    HoverableRadiusButton {
        id: btnRestart
        Layout.maximumWidth: 30
        Layout.preferredWidth: 30
        Layout.minimumWidth: 30

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        buttonImageHeight: height
        buttonImageWidth: height
        backgroundColor: "white"

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.right: btnRestart.left
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        radius: height / 2

        icon.source: "qrc:/images/icons/av_icons/re-record-24px.svg"
        icon.height: 24
        icon.width: 24
        onClicked: {
            stopRecording()
            updateState(RecordBox.States.INIT)
        }
    }

    HoverableRadiusButton {
        id: btnSend
        Layout.maximumWidth: 30
        Layout.preferredWidth: 30
        Layout.minimumWidth: 30

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        buttonImageHeight: height
        buttonImageWidth: height
        backgroundColor: "white"

        anchors.left: btnRestart.right
        anchors.leftMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5

        radius: height / 2

        icon.source: "qrc:/images/icons/av_icons/send-24px.svg"
        icon.height: 24
        icon.width: 24
        onClicked: {
            sendRecord()
            stopRecording()
            updateState(RecordBox.States.INIT)
        }
    }
}
