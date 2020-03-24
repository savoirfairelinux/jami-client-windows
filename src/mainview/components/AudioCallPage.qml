import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0
import net.jami.CallOverlayQmlObjectHolder 1.0

import "../../commoncomponents"

SplitView {
    id: audioCallPageSplitView

    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    property var corrspondingMessageWebView: null

    signal audioCallPageBackButtonIsClicked()

    function setCallOverlayBestName(bestName) {
        audioCallOverlay.bestName = bestName
    }

    function updateCallOverlay(accountId, convUid) {
        callOverlayQmlObjectHolder.updateCallOverlay(accountId, convUid)
    }

    function setAudioCallPageCorrspondingMessageWebView(webViewId) {
        corrspondingMessageWebView = webViewId
    }

    function closeInCallConversation() {
        if(inAudioCallMessageWebViewStack.visible) {
            corrspondingMessageWebView.setMessagingHeaderButtonsVisible(true)
            inAudioCallMessageWebViewStack.visible = false
            inAudioCallMessageWebViewStack.clear()
        }
    }

    anchors.fill: parent

    handle: Rectangle {
            implicitWidth: JamiTheme.splitViewHandlePreferedWidth
            implicitHeight: audioCallPageSplitView.height
            color: SplitHandle.pressed ? JamiTheme.pressColor : (SplitHandle.hovered ? JamiTheme.hoverColor : JamiTheme.tabbarBorderColor)
    }

    Rectangle {
        id: audioCallPageMainRect

        SplitView.minimumWidth: audioCallPageSplitView.width / 3
        SplitView.maximumWidth: audioCallPageSplitView.width
        SplitView.fillHeight: true

        CallOverlayQmlObjectHolder {
            id: callOverlayQmlObjectHolder

            onUpdateTimeText: {
                audioCallOverlay.timeText = time
            }

            onButtonStatusChanged: {
                audioCallOverlay.showOnHoldImage(isPaused)
                audioCallPageRectCentralRect.visible = !isPaused
                audioCallOverlay.updateButtonStatus(isPaused, isAudioOnly, isAudioMuted, isVideoMuted, isRecording)
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
                if(inAudioCallMessageWebViewStack.visible) {
                    corrspondingMessageWebView.setMessagingHeaderButtonsVisible(true)
                    inAudioCallMessageWebViewStack.visible = false
                    inAudioCallMessageWebViewStack.clear()
                }
                audioCallPageSplitView.audioCallPageBackButtonIsClicked()
            }

            onOverlayHangUpButtonClicked: {
                callOverlayQmlObjectHolder.hangUpThisCall()
            }

            onOverlayChatButtonClicked: {
                if(inAudioCallMessageWebViewStack.visible) {
                    corrspondingMessageWebView.setMessagingHeaderButtonsVisible(true)
                    inAudioCallMessageWebViewStack.visible = false
                    inAudioCallMessageWebViewStack.clear()
                } else {
                    corrspondingMessageWebView.setMessagingHeaderButtonsVisible(false)
                    inAudioCallMessageWebViewStack.visible = true
                    inAudioCallMessageWebViewStack.push(corrspondingMessageWebView)
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
                callOverlayQmlObjectHolder.setCallOverlayQmlObjectHolder(audioCallOverlay)
            }
        }

        Rectangle {
            id: audioCallPageRectCentralRect

            anchors.centerIn: parent

            width: audioCallPageSplitView.width
            height: audioCallPageRegisteredNameText.height + audioCallPageIdText.height + contactImage.height + 10

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
                        id:     textMetricsAudioCallPageRegisteredNameText
                        font:   audioCallPageRegisteredNameText.font
                        text:   bestName
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
                        id:     textMetricsAudioCallPageIdText
                        font:   audioCallPageIdText.font
                        text:   bestId
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

        SplitView.minimumWidth: audioCallPageSplitView.width / 3
        SplitView.maximumWidth: audioCallPageSplitView.width
        SplitView.fillHeight: true

        visible: false

        clip: true
    }

    background: Rectangle {
        color: "black"
    }
}
