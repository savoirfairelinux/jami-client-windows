import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0
import net.jami.CallOverlayQmlObjectHolder 1.0

import "../../commoncomponents"

Rectangle {
    id: audioCallPageRect

    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    property var corrspondingMessageWebView: null

    signal audioCallPageBackButtonIsClicked

    function setCallOverlayBestName(bestName) {
        audioCallOverlay.bestName = bestName
    }

    function updateCallOverlay(accountId, convUid) {
        callOverlayQmlObjectHolder.updateCallOverlay(accountId, convUid)
    }

    function setAudioCallPageCorrspondingMessageWebView(webViewId) {
        corrspondingMessageWebView = webViewId
        corrspondingMessageWebView.needToHideConversationInCall.disconnect(
                    closeInCallConversation)
        corrspondingMessageWebView.needToHideConversationInCall.connect(
                    closeInCallConversation)
    }

    function closeInCallConversation() {
        if (inAudioCallMessageWebViewStack.visible) {
            corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                        true)
            corrspondingMessageWebView.setMessagingHeaderButtonsVisible(true)
            inAudioCallMessageWebViewStack.visible = false
            inAudioCallMessageWebViewStack.clear()
        }
    }

    anchors.fill: parent

    SplitView {
        id: mainColumnLayout

        anchors.fill: parent

        orientation: Qt.Vertical

        handle: Rectangle {
            implicitWidth: audioCallPageRect.width
            implicitHeight: JamiTheme.splitViewHandlePreferedWidth
            color: SplitHandle.pressed ? JamiTheme.pressColor : (SplitHandle.hovered ? JamiTheme.hoverColor : JamiTheme.tabbarBorderColor)
        }

        Rectangle {
            id: audioCallPageMainRect

            SplitView.preferredHeight: (audioCallPageRect.height / 3) * 2
            SplitView.minimumHeight: audioCallPageRect.height / 2 + 20
            SplitView.fillWidth: true

            CallOverlayQmlObjectHolder {
                id: callOverlayQmlObjectHolder

                onUpdateTimeText: {
                    audioCallOverlay.timeText = time
                }

                onButtonStatusChanged: {
                    audioCallOverlay.showOnHoldImage(isPaused)
                    audioCallPageRectCentralRect.visible = !isPaused
                    audioCallOverlay.updateButtonStatus(isPaused, isAudioOnly,
                                                        isAudioMuted,
                                                        isVideoMuted,
                                                        isRecording)
                }

                onShowOnHoldLabel: {
                    audioCallOverlay.showOnHoldImage(isPaused)
                    audioCallPageRectCentralRect.visible = !isPaused
                }
            }

            CallOverlay {
                id: audioCallOverlay

                anchors.fill: parent

                onBackButtonIsClicked: {
                    if (inAudioCallMessageWebViewStack.visible) {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    true)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    true)
                        inAudioCallMessageWebViewStack.visible = false
                        inAudioCallMessageWebViewStack.clear()
                    }
                    audioCallPageRect.audioCallPageBackButtonIsClicked()
                }

                onOverlayHangUpButtonClicked: {
                    callOverlayQmlObjectHolder.hangUpThisCall()
                }

                onOverlayChatButtonClicked: {
                    if (inAudioCallMessageWebViewStack.visible) {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    true)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    true)
                        inAudioCallMessageWebViewStack.visible = false
                        inAudioCallMessageWebViewStack.clear()
                    } else {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    false)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    false)
                        inAudioCallMessageWebViewStack.visible = true
                        inAudioCallMessageWebViewStack.push(
                                    corrspondingMessageWebView)
                    }
                }

                onOverlayHoldButtonToggled: {
                    callOverlayQmlObjectHolder.holdThisCallToggle()
                }

                onOverlayNoMicButtonToggled: {
                    callOverlayQmlObjectHolder.muteThisCallToggle()
                }

                onOverlayRecButtonToggled: {
                    callOverlayQmlObjectHolder.recordThisCallToggle()
                }

                Component.onCompleted: {
                    callOverlayQmlObjectHolder.setCallOverlayQmlObjectHolder(
                                audioCallOverlay)
                }
            }

            Rectangle {
                id: audioCallPageRectCentralRect

                anchors.centerIn: parent

                width: audioCallPageRect.width
                height: audioCallPageRegisteredNameText.height
                        + audioCallPageIdText.height + contactImage.height + 10

                ColumnLayout {
                    id: audioCallPageRectColumnLayout

                    Image {
                        id: contactImage

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 100

                        fillMode: Image.PreserveAspectFit
                        source: contactImgSource
                        asynchronous: true
                    }

                    TextEdit {
                        id: audioCallPageRegisteredNameText

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: audioCallPageRectCentralRect.width
                        Layout.preferredHeight: 50

                        selectByMouse: true
                        readOnly: true
                        font.pointSize: JamiTheme.textFontSize + 3

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: textMetricsAudioCallPageRegisteredNameText.elidedText
                        color: "white"

                        TextMetrics {
                            id: textMetricsAudioCallPageRegisteredNameText
                            font: audioCallPageRegisteredNameText.font
                            text: bestName
                            elideWidth: audioCallPageRectCentralRect.width
                            elide: Qt.ElideMiddle
                        }
                    }

                    TextEdit {
                        id: audioCallPageIdText

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: audioCallPageRectCentralRect.width
                        Layout.preferredHeight: 30

                        selectByMouse: true
                        readOnly: true
                        font.pointSize: JamiTheme.textFontSize

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: textMetricsAudioCallPageIdText.elidedText
                        color: "white"

                        TextMetrics {
                            id: textMetricsAudioCallPageIdText
                            font: audioCallPageIdText.font
                            text: bestId
                            elideWidth: audioCallPageRectCentralRect.width
                            elide: Qt.ElideMiddle
                        }
                    }
                }

                color: "transparent"
            }

            color: "transparent"
        }

        StackView {
            id: inAudioCallMessageWebViewStack

            SplitView.preferredHeight: audioCallPageRect.height / 3
            SplitView.fillWidth: true

            visible: false

            clip: true
        }
    }

    color: "black"
}
