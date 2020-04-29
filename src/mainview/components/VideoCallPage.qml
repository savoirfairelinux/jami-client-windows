import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0
import net.jami.CallOverlayQmlObjectHolder 1.0
import net.jami.DistantRenderer 1.0
import net.jami.PreviewRenderer 1.0

import "../../commoncomponents"

Rectangle {
    id: videoCallPageRect

    property string bestName: "Best Name"
    property string bestId: "Best Id"

    property var corrspondingMessageWebView: null

    signal videoCallPageBackButtonIsClicked

    function updateUI(accountId, convUid) {
        bestName = utilsAdapter.getBestName(accountId, convUid)

        var id = utilsAdapter.getBestId(accountId, convUid)
        bestId = (bestName !== id) ? id : ""

        callOverlayQmlObjectHolder.updateCallOverlay(accountId, convUid)
    }

    function setDistantRendererId(id) {
        distantRenderer.setRendererId(id)
    }

    function setVideoCallPageCorrspondingMessageWebView(webViewId) {
        corrspondingMessageWebView = webViewId
        corrspondingMessageWebView.needToHideConversationInCall.disconnect(
                    closeInCallConversation)
        corrspondingMessageWebView.needToHideConversationInCall.connect(
                    closeInCallConversation)
    }

    function closeInCallConversation() {
        if (inVideoCallMessageWebViewStack.visible) {
            corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                        true)
            corrspondingMessageWebView.setMessagingHeaderButtonsVisible(true)
            inVideoCallMessageWebViewStack.visible = false
            inVideoCallMessageWebViewStack.clear()
        }
    }

    anchors.fill: parent

    SplitView {
        id: mainColumnLayout

        anchors.fill: parent

        orientation: Qt.Vertical

        handle: Rectangle {
            implicitWidth: videoCallPageRect.width
            implicitHeight: JamiTheme.splitViewHandlePreferedWidth
            color: SplitHandle.pressed ? JamiTheme.pressColor : (SplitHandle.hovered ? JamiTheme.hoverColor : JamiTheme.tabbarBorderColor)
        }

        Rectangle {
            id: videoCallPageMainRect

            SplitView.preferredHeight: (videoCallPageRect.height / 3) * 2
            SplitView.minimumHeight: videoCallPageRect.height / 2 + 20
            SplitView.fillWidth: true

            CallOverlayQmlObjectHolder {
                id: callOverlayQmlObjectHolder

                onUpdateTimeText: {
                    videoCallOverlay.timeText = time
                }

                onButtonStatusChanged: {
                    videoCallOverlay.showOnHoldImage(isPaused)
                    videoCallOverlay.updateButtonStatus(isPaused, isAudioOnly,
                                                        isAudioMuted,
                                                        isVideoMuted,
                                                        isRecording)
                }

                onShowOnHoldLabel: {
                    videoCallOverlay.showOnHoldImage(isPaused)
                }

                onUpdateBestName: {
                    videoCallOverlay.bestName = bestNameToBeUpdated
                }
            }

            CallOverlay {
                id: videoCallOverlay

                anchors.fill: parent

                onBackButtonIsClicked: {
                    if (inVideoCallMessageWebViewStack.visible) {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    true)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    true)
                        inVideoCallMessageWebViewStack.visible = false
                        inVideoCallMessageWebViewStack.clear()
                    }
                    videoCallPageRect.videoCallPageBackButtonIsClicked()
                }

                onOverlayHangUpButtonClicked: {
                    callOverlayQmlObjectHolder.hangUpThisCall()
                }

                onOverlayChatButtonClicked: {
                    if (inVideoCallMessageWebViewStack.visible) {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    true)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    true)
                        inVideoCallMessageWebViewStack.visible = false
                        inVideoCallMessageWebViewStack.clear()
                    } else {
                        corrspondingMessageWebView.resetMessagingHeaderBackButtonSource(
                                    false)
                        corrspondingMessageWebView.setMessagingHeaderButtonsVisible(
                                    false)
                        inVideoCallMessageWebViewStack.visible = true
                        inVideoCallMessageWebViewStack.push(
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
                                videoCallOverlay)
                }
            }

            DistantRenderer {
                id: distantRenderer

                anchors.centerIn: videoCallPageMainRect
                z: -1

                width: videoCallPageMainRect.width
                height: videoCallPageMainRect.height
            }

            color: "transparent"
        }

        StackView {
            id: inVideoCallMessageWebViewStack

            SplitView.preferredHeight: videoCallPageRect.height / 3
            SplitView.fillWidth: true

            visible: false

            clip: true
        }
    }

    color: "black"
}
