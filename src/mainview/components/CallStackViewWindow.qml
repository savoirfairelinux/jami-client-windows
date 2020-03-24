import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0

Window {
    id: callStackViewWindow

    property int minWidth: 300
    property int minHeight: 400

    property string responsibleConvUid: ""
    property string responsibleAccountId: ""

    title: "Incoming Call"
    visible: true

    minimumWidth: minWidth
    minimumHeight: minHeight

    maximumWidth: minWidth + 300
    maximumHeight: minHeight + 300

    //flags: Qt.SubWindow | Qt.Tool | Qt.FramelessWindowHint | Qt.WindowSystemMenuHint | Qt.WindowStaysOnTopHint

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
                callStackViewWindow.show()
            }
        }

        onShowIncomingCallPage: {
            var itemToFind = callStackMainView.find(function(item) {
                return item.stackNumber === 2;
            })

            if(!itemToFind) {
                callStackMainView.push(incomingCallPage)
            } else {
                callStackMainView.pop(itemToFind)
            }
            callStackViewWindow.show()
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
                callStackViewWindow.show()
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
    }

    OutgoingCallPage {
        id: outgoingCallPage

        property int stackNumber: 1
    }

    IncomingCallPage {
        id: incomingCallPage

        property int stackNumber: 2

        onCallAnswerButtonIsClicked: {
            callStackMainView.push(audioCallPage)
        }
    }

    StackView {
        id: callStackMainView

        anchors.fill: parent

        initialItem: outgoingCallPage
    }
}
