import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtQml 2.14
import net.jami.JamiTheme 1.0
import net.jami.CallAdapter 1.0

import "../../commoncomponents"

Rectangle {
    id: callOverlayButtonGroupRect

    property int buttonCounts: 9
    property int buttonPreferredSize: 30

    signal buttonEntered
    signal chatButtonClicked

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

    function calculateFlowMargin() {
        return (callOverlayButtonGroupRect.width - buttonCounts * buttonPreferredSize
                - callOverlayButtonGroupRectFlow.spacing * (buttonCounts - 1)) / 2
    }

    Flow {
        id: callOverlayButtonGroupRectFlow

        anchors.fill: parent
        anchors.leftMargin: calculateFlowMargin(
                                ) < 0 ? 0 : calculateFlowMargin()
        anchors.rightMargin: calculateFlowMargin(
                                 ) < 0 ? 0 : calculateFlowMargin()

        spacing: 10

        TintedButton {
            id: hangUpButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.hangUpButtonTintedRed
            normalPixmapSource: "qrc:/images/icons/ic_close_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_close_white_24dp.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                CallAdapter.hangUpThisCall()
            }

            onVisibleChanged: {
                if (this.visible)
                    buttonCounts++
                else
                    buttonCounts--
            }
        }

        TintedButton {
            id: holdButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_pause_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_play_white_24dp.png"

            onClicked: {
                CallAdapter.holdThisCallToggle()
            }

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

        TintedButton {
            id: addToConferenceButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_group_add_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_group_add_white_24dp.png"

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

            onVisibleChanged: {
                if (this.visible)
                    buttonCounts++
                else
                    buttonCounts--
            }
        }

        TintedButton {
            id: chatButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_chat_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_chat_white_24dp.png"

            onClicked: {
                callOverlayButtonGroupRect.chatButtonClicked()
            }

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

        TintedButton {
            id: noMicButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_mic_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_mic_off_white_24dp.png"

            onClicked: {
                CallAdapter.muteThisCallToggle()
            }

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

        TintedButton {
            id: noVideoButton

            width: buttonPreferredSize
            height: buttonPreferredSize

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_videocam_white.png"
            selectedPixmapSource: "qrc:/images/icons/ic_videocam_off_white_24dp.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                CallAdapter.videoPauseThisCallToggle()
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
    }

    color: "transparent"
}
