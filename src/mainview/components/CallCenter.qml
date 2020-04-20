pragma Singleton

import QtQuick 2.14
import QtQuick.Controls 2.14
import net.jami.CallCenterQmlObjectHolder 1.0

//https://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType-3
Item {
    id: callCenter

    signal showCallStack(string accountId, string convUid)
    signal closeCallStack(string accountId, string convUid)
    signal closePotentialIncomingCallPageWindow(string accountId, string convUid)
    signal incomingCallNeedToSetupCallStackView(string accountId, string convUid)

    signal showIncomingCallPage(string accountId, string convUid)
    signal showOutgoingCallPage(string accountId, string convUid)
    signal showAudioCallPage(string accountId, string convUid)

    signal callStatusChanged(string status, string accountId, string convUid)
    signal callContactImageChanged(string image, string accountId, string convUid)
    signal setUIBestName(string bestName, string accountId, string convUid)
    signal setUIBestId(string bestId, string accountId, string convUid)
    signal needToUpdateConversationSmartList

    function init() {}

    function placeAudioOnlyCall() {
        callCenterQmlObjectHolder.placeAudioOnlyCall()
    }

    function hangUpACall(accountId, convUid) {
        callCenterQmlObjectHolder.hangUpACall(accountId, convUid)
    }

    function refuseACall(accountId, convUid) {
        callCenterQmlObjectHolder.refuseACall(accountId, convUid)
    }

    function acceptACall(accountId, convUid) {
        callCenter.incomingCallNeedToSetupCallStackView(accountId, convUid)
        callCenterQmlObjectHolder.acceptACall(accountId, convUid)
    }

    function connectCallstatusChangedSignal(accountId) {
        callCenterQmlObjectHolder.connectCallstatusChangedSignal(accountId)
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
            callCenter.showOutgoingCallPage(accountId, convUid)
            callCenter.showCallStack(accountId, convUid)
        }

        onShowIncomingCallPage: {
            callCenter.showIncomingCallPage(accountId, convUid)
        }

        onShowAudioCallPage: {
            callCenter.showAudioCallPage(accountId, convUid)
            callCenter.showCallStack(accountId, convUid)
        }

        onCloseCallWindow: {
            callCenter.closeCallStack(accountId, convUid)
            callCenter.closePotentialIncomingCallPageWindow(accountId, convUid)
        }

        onSetUIBestName: {
            callCenter.setUIBestName(bestName, accountId, convUid)
        }

        onSetUIBestId: {
            callCenter.setUIBestId(bestId, accountId, convUid)
        }

        onUpdateConversationSmartList: {
            callCenter.needToUpdateConversationSmartList()
        }
    }

    Component.onCompleted: {
        callCenterQmlObjectHolder.setCallCenterQmlObjectHolder(callCenter)
    }
}
