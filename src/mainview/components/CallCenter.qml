pragma Singleton

import QtQuick 2.14
import QtQuick.Controls 2.14
import net.jami.CallCenterQmlObjectHolder 1.0
import "js/callWindowCreation.js" as CallWindowCreationScript

//https://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType-3

Item {
    id: callCenter

    signal showIncomingCallPage()
    signal showOutgoingCallPage(string accountId, string convUid)
    signal showAudioCallPage(string accountId, string convUid)

    signal callStatusChanged(string status, string accountId, string convUid)
    signal callContactImageChanged(string image, string accountId, string convUid)
    signal setUIBestName(string bestName, string accountId, string convUid)
    signal setUIBestId(string bestId, string accountId, string convUid)

    signal setCallPanelVisibility(bool visible)

    function init() {
    }

    function placeAudioOnlyCall() {
        callCenterQmlObjectHolder.placeAudioOnlyCall()
    }

    function hangUpACall(accountId, convUid) {
        callCenterQmlObjectHolder.hangUpACall(accountId, convUid)
    }

    CallCenterQmlObjectHolder {
        id: callCenterQmlObjectHolder

        onCallStatusChanged: {
            callCenter.callStatusChanged(status, accountId, convUid)
        }

        onCallContactImageChanged: {
            callCenter.callContactImageChanged(imageString, accountId, convUid)
        }

        onShowOutgoingCallPage: {
            CallWindowCreationScript.createCallWindowObjects(accountId, convUid)
            CallWindowCreationScript.showCallWindow(accountId, convUid)
            callCenter.showOutgoingCallPage(accountId, convUid)
        }

        onShowIncomingCallPage: {
            callCenter.showIncomingCallPage()
        }

        onShowAudioCallPage: {
            callCenter.showAudioCallPage(accountId, convUid)
        }

        onCloseCallWindow: {
            CallWindowCreationScript.closeCallWindow(accountId, convUid)
        }

        onSetUIBestName: {
            callCenter.setUIBestName(bestName, accountId, convUid)
        }

        onSetUIBestId: {
            callCenter.setUIBestId(bestId, accountId, convUid)
        }

        onSetCallPanelVisibility: {
            callCenter.setCallPanelVisibility(visible)
        }
    }

    Component.onCompleted: {
        callCenterQmlObjectHolder.setCallCenterQmlObjectHolder(callCenter)
    }
}
