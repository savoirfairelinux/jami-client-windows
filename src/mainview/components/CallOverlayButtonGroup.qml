
/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
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
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtQml 2.14
import net.jami.Models 1.0

import "../../commoncomponents"

Rectangle {
    id: callOverlayButtonGroupRect

    /*
     * ButtonCounts here is to make sure that flow layout margin is calculated correctly,
     * since no other methods can make buttons at the layout center.
     */
    property int buttonPreferredSize: 40

    signal buttonEntered
    signal chatButtonClicked
    signal addToConferenceButtonClicked
    signal transferCallButtonClicked

    color: "transparent"

    function setButtonStatus(isPaused, isAudioOnly, isAudioMuted, isVideoMuted, isRecording, isSIP, isConferenceCall) {
        noVideoButton.visible = !isAudioOnly
        addToConferenceButton.visible = !isSIP
        transferCallButton.visible = isSIP
        sipInputPanelButton.visible = isSIP

        noMicButton.setChecked(isAudioMuted)
        noVideoButton.setChecked(isVideoMuted)
        recButton.setChecked(isRecording)
        holdButton.setChecked(isPaused)

        holdButton.visible = !isConferenceCall
    }

    Row {
        id: callOverlayButtonGroup

        anchors.centerIn: parent

        spacing: 10

        TintedButton {
            id: noMicButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.declineButtonHoverRed
            normalPixmapSource: "qrc:/images/icons/ic_mic_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_mic_off_white_24dp.png"

            onClicked: {
                CallAdapter.muteThisCallToggle()
            }

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }

        HoverableButton {
            id: hangUpButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            backgroundColor: JamiTheme.declineButtonRed
            onEnterColor: JamiTheme.declineButtonHoverRed
            onPressColor: JamiTheme.declineButtonPressedRed
            onReleaseColor: JamiTheme.declineButtonHoverRed
            onExitColor: JamiTheme.declineButtonRed

            buttonImageHeight: buttonPreferredSize / 2
            buttonImageWidth: buttonPreferredSize / 2
            source: "qrc:/images/icons/ic_call_end_white_24px.svg"
            radius: 30

            onClicked: {
                CallAdapter.hangUpThisCall()
            }
        }

        TintedButton {
            id: noVideoButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.declineButtonHoverRed
            normalPixmapSource: "qrc:/images/icons/ic_videocam_white.png"
            selectedPixmapSource: "qrc:/images/icons/ic_videocam_off_white_24dp.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                CallAdapter.videoPauseThisCallToggle()
            }
        }
    }

    Row {
        id: callOverlayButtonGroup2

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        spacing: 10

        HoverableButton {
            id: addToConferenceButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            backgroundColor: Qt.rgba(0, 0, 0, 0.8)
            onEnterColor: Qt.rgba(10, 10, 10, 0.8)
            onPressColor: Qt.rgba(20, 20, 20, 0.8)
            onReleaseColor: Qt.rgba(0, 0, 0, 0.8)
            onExitColor: Qt.rgba(0, 0, 0, 0.8)

            buttonImageHeight: buttonPreferredSize / 2
            buttonImageWidth: buttonPreferredSize / 2
            source: "qrc:/images/icons/ic_group_add_white_24dp.png"
            radius: 30

            onClicked: {
                callOverlayButtonGroupRect.addToConferenceButtonClicked()
            }
        }

        HoverableButton {
            id: chatButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            backgroundColor: Qt.rgba(0, 0, 0, 0.8)
            onEnterColor: Qt.rgba(10, 10, 10, 0.8)
            onPressColor: Qt.rgba(20, 20, 20, 0.8)
            onReleaseColor: Qt.rgba(0, 0, 0, 0.8)
            onExitColor: Qt.rgba(0, 0, 0, 0.8)

            buttonImageHeight: buttonPreferredSize / 2
            buttonImageWidth: buttonPreferredSize / 2
            source: "qrc:/images/icons/ic_chat_white_24dp.png"
            radius: 30

            onClicked: {
                callOverlayButtonGroupRect.chatButtonClicked()
            }
        }

        HoverableButton {
            id: optionsButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            backgroundColor: Qt.rgba(0, 0, 0, 0.8)
            onEnterColor: Qt.rgba(10, 10, 10, 0.8)
            onPressColor: Qt.rgba(20, 20, 20, 0.8)
            onReleaseColor: Qt.rgba(0, 0, 0, 0.8)
            onExitColor: Qt.rgba(0, 0, 0, 0.8)

            buttonImageHeight: buttonPreferredSize / 2
            buttonImageWidth: buttonPreferredSize / 2
            source: "qrc:/images/icons/more_vert-24px.svg"
            radius: 30

            onClicked: {
                var rectPos = mapToItem(videoCallPageRect, optionsButton.x, optionsButton.y)
                callViewContextMenu.activate()
                callViewContextMenu.x = rectPos.x + optionsButton.width/2 - callViewContextMenu.width/2
                callViewContextMenu.y = rectPos.y - 12 - callViewContextMenu.height
            }
        }
    }



/*

        TintedButton {
            id: transferCallButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_call_transfer_white_24px.png"
            selectedPixmapSource: "qrc:/images/icons/ic_call_transfer_white_24px.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                callOverlayButtonGroupRect.transferCallButtonClicked()
            }

            onVisibleChanged: {
                if (this.visible)
                    buttonCounts++
                else
                    buttonCounts--
            }
        }

        TintedButton {
            id: recButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_voicemail_white_24dp_2x.png"
            selectedPixmapSource: "qrc:/images/icons/ic_voicemail_white_24dp_2x.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                CallAdapter.recordThisCallToggle()
            }

            onVisibleChanged: {
                if (this.visible)
                    buttonCounts++
                else
                    buttonCounts--
            }
        }

        TintedButton {
            id: sipInputPanelButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/icon-keypad-24-2x.png"
            selectedPixmapSource: "qrc:/images/icons/icon-keypad-24-2x.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onVisibleChanged: {
                if (this.visible)
                    buttonCounts++
                else
                    buttonCounts--
            }
        }
        */
}
