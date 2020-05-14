
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
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.JamiTheme 1.0
import net.jami.CallAdapter 1.0

import "../js/incomingcallpagecreation.js" as IncomingCallPageCreation
import "../js/videocallfullscreenwindowcontainercreation.js" as VideoCallFullScreenWindowContainerCreation

Rectangle {
    id: callStackViewWindow

    anchors.fill: parent


    /*
     * When selected conversation is changed,
     * these values will also be changed.
     */
    property string responsibleConvUid: ""
    property string responsibleAccountId: ""

    signal outgoingCallPageBackButtonIsClicked
    signal callPageBackButtonIsClicked

    function needToCloseInCallConversationAndPotentialWindow() {
        audioCallPage.closeInCallConversation()
        videoCallPage.closeInCallConversation()


        /*
         * Close potential window, context menu releated windows.
         */
        if (VideoCallFullScreenWindowContainerCreation.getObject())
            VideoCallFullScreenWindowContainerCreation.getObject().close()
        videoCallPage.closeContextMenuAndRelatedWindows()
    }

    function setCorrspondingMessageWebView(webViewId) {
        audioCallPage.setAudioCallPageCorrspondingMessageWebView(webViewId)
        videoCallPage.setVideoCallPageCorrspondingMessageWebView(webViewId)
    }

    function updateCorrspondingUI() {
        audioCallPage.updateUI(responsibleAccountId, responsibleConvUid)
        outgoingCallPage.updateUI(responsibleAccountId, responsibleConvUid)
        videoCallPage.updateUI(responsibleAccountId, responsibleConvUid)
    }

    Connections {
        target: CallAdapter

        onShowOutgoingCallPage: {


            /*
             * Need to check whether it is the current selected conversation.
             */
            if (responsibleConvUid == convUid
                    && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function (item) {
                    return item.stackNumber === 1
                })

                if (!itemToFind) {
                    callStackMainView.push(outgoingCallPage,
                                           StackView.Immediate)
                } else {
                    callStackMainView.pop(itemToFind, StackView.Immediate)
                }
            }
        }

        onShowIncomingCallPage: {


            /*
             * Check is done within the js
             */
            IncomingCallPageCreation.createincomingCallPageWindowObjects(
                        accountId, convUid)
            IncomingCallPageCreation.showIncomingCallPageWindow(accountId,
                                                                convUid)
        }

        onClosePotentialIncomingCallPageWindow: {
            IncomingCallPageCreation.closeIncomingCallPageWindow(accountId,
                                                                 convUid)
        }

        onShowAudioCallPage: {
            if (responsibleConvUid == convUid
                    && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function (item) {
                    return item.stackNumber === 0
                })

                if (!itemToFind) {
                    callStackMainView.push(audioCallPage, StackView.Immediate)
                } else {
                    callStackMainView.pop(itemToFind, StackView.Immediate)
                }
                audioCallPage.updateUI(responsibleAccountId, responsibleConvUid)
            }
        }

        onShowVideoCallPage: {
            if (responsibleConvUid == convUid
                    && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function (item) {
                    return item.stackNumber === 2
                })

                if (!itemToFind) {
                    callStackMainView.push(videoCallPage, StackView.Immediate)
                } else {
                    callStackMainView.pop(itemToFind, StackView.Immediate)
                }
                videoCallPage.updateUI(responsibleAccountId, responsibleConvUid)
                videoCallPage.setDistantRendererId(callId)
            }
        }

        onCallStatusChanged: {
            if (responsibleConvUid == convUid
                    && responsibleAccountId == accountId) {
                outgoingCallPage.callStatusPresentation = status
            }
        }
    }

    AudioCallPage {
        id: audioCallPage

        property int stackNumber: 0

        onAudioCallPageBackButtonIsClicked: {
            callStackViewWindow.callPageBackButtonIsClicked()
        }
    }

    OutgoingCallPage {
        id: outgoingCallPage

        property int stackNumber: 1

        onCallCancelButtonIsClicked: {
            CallAdapter.hangUpACall(responsibleAccountId, responsibleConvUid)
        }

        onBackButtonIsClicked: {
            callStackViewWindow.outgoingCallPageBackButtonIsClicked()
        }
    }

    VideoCallPage {
        id: videoCallPage

        property int stackNumber: 2

        onVideoCallPageBackButtonIsClicked: {
            callStackViewWindow.callPageBackButtonIsClicked()
        }

        onNeedToShowInFullScreen: {
            if (!VideoCallFullScreenWindowContainerCreation.getObject())
                VideoCallFullScreenWindowContainerCreation.createvideoCallFullScreenWindowContainerObject()

            if (!VideoCallFullScreenWindowContainerCreation.getObject(
                        ).visible) {
                VideoCallFullScreenWindowContainerCreation.getObject(
                            ).setAsChild(videoCallPage)


                /*
                 * Hack: show first, then showFullScreen to make sure that the showFullScreen
                 * display on the correct screen.
                 */
                VideoCallFullScreenWindowContainerCreation.getObject().show()
                VideoCallFullScreenWindowContainerCreation.getObject(
                            ).showFullScreen()
            } else {
                videoCallPage.parent = callStackMainView
                VideoCallFullScreenWindowContainerCreation.getObject().close()
            }
        }
    }

    StackView {
        id: callStackMainView

        anchors.fill: parent

        initialItem: outgoingCallPage
    }
}
