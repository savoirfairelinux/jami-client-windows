import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0

import "../js/incomingcallpagecreation.js" as IncomingCallPageCreation

Rectangle {
    id: callStackViewWindow

    anchors.fill: parent

    property string responsibleConvUid: ""
    property string responsibleAccountId: ""

    signal outgoingCallPageBackButtonIsClicked
    signal callPageBackButtonIsClicked

    function needToCloseInCallConversation() {
        audioCallPage.closeInCallConversation()
        videoCallPage.closeInCallConversation()
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
        target: CallCenter

        onShowOutgoingCallPage: {
            if (responsibleConvUid == convUid
                    && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function (item) {
                    return item.stackNumber === 1
                })

                if (!itemToFind) {
                    callStackMainView.push(outgoingCallPage)
                } else {
                    callStackMainView.pop(itemToFind)
                }
            }
        }

        onShowIncomingCallPage: {
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
                    callStackMainView.push(audioCallPage)
                } else {
                    callStackMainView.pop(itemToFind)
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
                    callStackMainView.push(videoCallPage)
                } else {
                    callStackMainView.pop(itemToFind)
                }
                videoCallPage.updateUI(responsibleAccountId,
                                       responsibleConvUid)
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
            CallCenter.hangUpACall(responsibleAccountId, responsibleConvUid)
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
    }

    StackView {
        id: callStackMainView

        anchors.fill: parent

        initialItem: outgoingCallPage
    }
}
