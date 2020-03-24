import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0
import net.jami.CallOverlayQmlObjectHolder 1.0

import "../../commoncomponents"

Rectangle {
    id: audioCallPageRect

    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    function setCallOverlayBestName(bestName) {
        audioCallOverlay.bestName = bestName
    }

    function updateCallOverlay(accountId, convUid) {
        callOverlayQmlObjectHolder.updateCallOverlay(accountId, convUid)
    }

    anchors.fill: parent

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

        onOverlayHangUpButtonClicked: {
            callOverlayQmlObjectHolder.hangUpThisCall()
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

        width: audioCallPageRect.width
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

        color: "black"
    }

    color: "black"
}
