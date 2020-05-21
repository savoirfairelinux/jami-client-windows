
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
import net.jami.JamiTheme 1.0
import net.jami.CallAdapter 1.0

import "../../commoncomponents"

Rectangle {
    id: audioCallPageRect

    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    property var corrspondingMessageWebView: null

    signal audioCallPageBackButtonIsClicked

    function updateUI(accountId, convUid) {
        contactImgSource = "data:image/png;base64," + utilsAdapter.getContactImageString(
                    accountId, convUid)
        bestName = utilsAdapter.getBestName(accountId, convUid)

        var id = utilsAdapter.getBestId(accountId, convUid)
        bestId = (bestName !== id) ? id : ""

        CallAdapter.updateCallOverlay(accountId, convUid)
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

            CallOverlay {
                id: audioCallOverlay

                anchors.fill: parent

                Connections {
                    target: CallAdapter

                    onUpdateTimeText: {
                        audioCallOverlay.timeText = time
                    }

                    onButtonStatusChanged: {
                        audioCallOverlay.showOnHoldImage(isPaused)
                        audioCallPageRectCentralRect.visible = !isPaused
                        audioCallOverlay.updateButtonStatus(isPaused,
                                                            isAudioOnly,
                                                            isAudioMuted,
                                                            isVideoMuted,
                                                            isRecording, isSIP,
                                                            isConferenceCall)
                    }

                    onShowOnHoldLabel: {
                        audioCallOverlay.showOnHoldImage(isPaused)
                        audioCallPageRectCentralRect.visible = !isPaused
                    }

                    onUpdateBestName: {
                        audioCallOverlay.bestName = bestNameToBeUpdated
                    }
                }

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

                    Text {
                        id: audioCallPageRegisteredNameText

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: audioCallPageRectCentralRect.width
                        Layout.preferredHeight: 50

                        font.pointSize: JamiTheme.textFontSize + 3

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: textMetricsAudioCallPageRegisteredNameText.elidedText
                        color: "white"

                        TextMetrics {
                            id: textMetricsAudioCallPageRegisteredNameText
                            font: audioCallPageRegisteredNameText.font
                            text: bestName
                            elideWidth: audioCallPageRectCentralRect.width - 50
                            elide: Qt.ElideMiddle
                        }
                    }

                    Text {
                        id: audioCallPageIdText

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: audioCallPageRectCentralRect.width
                        Layout.preferredHeight: 30

                        font.pointSize: JamiTheme.textFontSize

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: textMetricsAudioCallPageIdText.elidedText
                        color: "white"

                        TextMetrics {
                            id: textMetricsAudioCallPageIdText
                            font: audioCallPageIdText.font
                            text: bestId
                            elideWidth: audioCallPageRectCentralRect.width - 50
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
