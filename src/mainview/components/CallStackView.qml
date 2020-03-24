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

    signal outgoingCallPageBackButtonIsClicked()
    signal audioCallPageBackButtonIsClicked()

    function needToCloseInCallConversation() {
        audioCallPage.closeInCallConversation()
    }

    function setCorrspondingMessageWebView(webViewId) {
        audioCallPage.setAudioCallPageCorrspondingMessageWebView(webViewId)
    }

    Connections {
        target: CallCenter

        onShowOutgoingCallPage: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function(item) {
                    return item.stackNumber === 1;
                })

                if(!itemToFind) {
                    callStackMainView.push(outgoingCallPage)
                } else {
                    callStackMainView.pop(itemToFind)
                }
            }
        }

        onShowIncomingCallPage: {
            IncomingCallPageCreation.createincomingCallPageWindowObjects(accountId, convUid)
            IncomingCallPageCreation.showIncomingCallPageWindow(accountId, convUid)
        }

        onShowAudioCallPage: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                var itemToFind = callStackMainView.find(function(item) {
                    return item.stackNumber === 0;
                })

                if(!itemToFind) {
                    callStackMainView.push(audioCallPage)
                } else {
                    callStackMainView.pop(itemToFind)
                }
                audioCallPage.updateCallOverlay(responsibleAccountId, responsibleConvUid)
            }
        }

        onCallStatusChanged: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                outgoingCallPage.callStatusPresentation = status
            }
        }

        onCallContactImageChanged: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                outgoingCallPage.contactImgSource = "data:image/png;base64," + image
                audioCallPage.contactImgSource = "data:image/png;base64," + image
            }
        }

        onSetUIBestName: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                outgoingCallPage.bestName = bestName
                audioCallPage.bestName = bestName
                audioCallPage.setCallOverlayBestName(bestName)
            }
        }

        onSetUIBestId: {
            if(responsibleConvUid == convUid && responsibleAccountId == accountId) {
                outgoingCallPage.bestId = bestId
                audioCallPage.bestId = bestId
            }
        }
    }

    AudioCallPage {
        id: audioCallPage

        property int stackNumber: 0

        onAudioCallPageBackButtonIsClicked: {
            callStackViewWindow.audioCallPageBackButtonIsClicked()
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

    StackView {
        id: callStackMainView

        anchors.fill: parent

        initialItem: outgoingCallPage
    }
}
