import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.constant.jamitheme 1.0
import net.jami.CallOverlayQmlObjectHolder 1.0
import net.jami.DistantRenderer 1.0
import net.jami.VideoCallPreviewRenderer 1.0

import "../../commoncomponents"

Rectangle {
    id: videoCallPageRect

    property string bestName: "Best Name"
    property string bestId: "Best Id"
    property variant clickPos: "1,1"
    property int previewMargin: 15
    property int previewToX: 0
    property int previewToY: 0

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

    function previewMagneticSnap() {
        // Calculate the position where the previewRenderer should attach to
        var previewRendererCenter = Qt.point(
                    previewRenderer.x + previewRenderer.width / 2,
                    previewRenderer.y + previewRenderer.height / 2)
        var distantRendererCenter = Qt.point(
                    distantRenderer.x + distantRenderer.width / 2,
                    distantRenderer.y + distantRenderer.height / 2)

        if (previewRendererCenter.x >= distantRendererCenter.x) {
            if (previewRendererCenter.y >= distantRendererCenter.y) {
                // Bottom right
                previewToX = Qt.binding(function () {
                    return videoCallPageMainRect.width - previewRenderer.width - previewMargin
                })
                previewToY = Qt.binding(function () {
                    return videoCallPageMainRect.height - previewRenderer.height - previewMargin
                })
            } else {
                // Top right
                previewToX = Qt.binding(function () {
                    return videoCallPageMainRect.width - previewRenderer.width - previewMargin
                })
                previewToY = previewMargin
            }
        } else {
            if (previewRendererCenter.y >= distantRendererCenter.y) {
                // Bottom left
                previewToX = previewMargin
                previewToY = Qt.binding(function () {
                    return videoCallPageMainRect.height - previewRenderer.height - previewMargin
                })
            } else {
                // Top left
                previewToX = previewMargin
                previewToY = previewMargin
            }
        }
        previewRenderer.state = "geoChanging"
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

            VideoCallPreviewRenderer {
                id: previewRenderer


                /* Property is used in the {} expression for height (extra dependency),
                 * it will not affect the true height expression, since expression
                 * at last will be taken only, but it will force the height to update
                 * and reevaluate getPreviewImageScalingFactor().
                 */
                property int previewImageScalingFactorUpdated: 0

                width: videoCallPageMainRect.width / 4
                height: {
                    previewImageScalingFactorUpdated
                    return previewRenderer.width * previewRenderer.getPreviewImageScalingFactor()
                }
                x: videoCallPageMainRect.width - previewRenderer.width - previewMargin
                y: videoCallPageMainRect.height - previewRenderer.height - previewMargin
                z: -1

                /*layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle {
                        id: mask

                        width: previewRenderer.width
                        height: previewRenderer.height

                        radius: 15
                    }
                }*/

                states: [
                    State {
                        name: "geoChanging"
                        PropertyChanges {
                            target: previewRenderer
                            x: previewToX
                            y: previewToY
                        }
                    }
                ]

                transitions: Transition {
                    PropertyAnimation {
                        properties: "x,y"
                        easing.type: Easing.OutExpo
                        duration: 250

                        onStopped: {
                            previewRenderer.state = ""
                        }
                    }
                }

                MouseArea {
                    id: dragMouseArea

                    anchors.fill: previewRenderer

                    onPressed: {
                        clickPos = Qt.point(mouse.x, mouse.y)
                    }

                    onReleased: {
                        previewRenderer.state = ""
                        previewMagneticSnap()
                    }

                    onPositionChanged: {
                        // Calculate mouse position relative change
                        var delta = Qt.point(mouse.x - clickPos.x,
                                             mouse.y - clickPos.y)
                        var deltaW = previewRenderer.x + delta.x + previewRenderer.width
                        var deltaH = previewRenderer.y + delta.y + previewRenderer.height

                        // Check if the previewRenderer exceeds the border of videoCallPageMainRect
                        if (deltaW < videoCallPageMainRect.width
                                && previewRenderer.x + delta.x > 1)
                            previewRenderer.x += delta.x
                        if (deltaH < videoCallPageMainRect.height
                                && previewRenderer.y + delta.y > 1)
                            previewRenderer.y += delta.y
                    }
                }

                onPreviewImageAvailable: {
                    previewImageScalingFactorUpdated++
                    previewImageScalingFactorUpdated--
                }
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
