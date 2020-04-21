import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtQml 2.14
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Rectangle {
    id: callOverlayRect

    property string bestName: "Best Name"
    property string timeText: "00:00"

    signal backButtonIsClicked

    signal overlayHangUpButtonClicked
    signal overlayChatButtonClicked

    signal overlayHoldButtonToggled
    signal overlayNoMicButtonToggled
    signal overlayRecButtonToggled

    function updateButtonStatus(isPaused, isAudioOnly, isAudioMuted, isVideoMuted, isRecording) {
        callOverlayButtonGroup.holdButtonSetChecked(isPaused)
    }

    function showOnHoldImage(visible) {
        onHoldImage.visible = visible
    }

    anchors.fill: parent

    Timer {
        id: callOverlayTimer
        interval: 5000
        onTriggered: {
            if (overlayUpperPartRect.state !== 'freezed') {
                overlayUpperPartRect.state = 'freezed'
            }
            if (callOverlayButtonGroup.state !== 'freezed') {
                callOverlayButtonGroup.state = 'freezed'
            }
        }
    }

    Rectangle {
        id: overlayUpperPartRect

        anchors.top: callOverlayRect.top

        width: callOverlayRect.width
        height: 50
        opacity: 0

        RowLayout {
            id: overlayUpperPartRectRowLayout

            anchors.fill: parent

            TintedButton {
                id: backTintedButton

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.leftMargin: 5
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                tintColor: JamiTheme.buttonTintedBlue
                normalPixmapSource: "qrc:/images/icons/ic_arrow_back_white_24dp.png"
                selectedPixmapSource: "qrc:/images/icons/ic_arrow_back_white_24dp.png"

                onClicked: {
                    callOverlayRect.backButtonIsClicked()
                }

                onButtonEntered: {
                    callOverlayRectMouseArea.entered()
                }
            }

            TextEdit {
                id: jamiBestNameText

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.preferredWidth: overlayUpperPartRect.width / 3
                Layout.preferredHeight: 50

                selectByMouse: false
                readOnly: true
                font.pointSize: JamiTheme.textFontSize

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiBestNameText.elidedText
                color: "white"

                TextMetrics {
                    id: textMetricsjamiBestNameText
                    font: jamiBestNameText.font
                    text: bestName
                    elideWidth: overlayUpperPartRect.width / 3
                    elide: Qt.ElideMiddle
                }
            }

            TextEdit {
                id: callTimerText

                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.preferredWidth: overlayUpperPartRect.width / 3
                Layout.preferredHeight: 50

                selectByMouse: false
                readOnly: true
                font.pointSize: JamiTheme.textFontSize

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricscallTimerText.elidedText
                color: "white"

                TextMetrics {
                    id: textMetricscallTimerText
                    font: callTimerText.font
                    text: timeText
                    elideWidth: overlayUpperPartRect.width / 3
                    elide: Qt.ElideMiddle
                }
            }
        }

        color: "transparent"

        states: [
            State {
                name: "entered"
                PropertyChanges {
                    target: overlayUpperPartRect
                    opacity: 1
                }
            },
            State {
                name: "freezed"
                PropertyChanges {
                    target: overlayUpperPartRect
                    opacity: 0
                }
            }
        ]

        transitions: Transition {
            PropertyAnimation {
                target: overlayUpperPartRect
                property: "opacity"
                duration: 1000
            }
        }
    }

    Image {
        id: onHoldImage

        anchors.verticalCenter: callOverlayRect.verticalCenter
        anchors.horizontalCenter: callOverlayRect.horizontalCenter

        width: 200
        height: 200

        visible: false

        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icons/ic_pause_white_100px.png"
        asynchronous: true
    }

    CallOverlayButtonGroup {
        id: callOverlayButtonGroup

        anchors.bottom: callOverlayRect.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: callOverlayRect.horizontalCenter

        width: callOverlayRect.width / 3 * 2
        height: 60
        opacity: 0

        onHangUpButtonClicked: {
            callOverlayRect.overlayHangUpButtonClicked()
        }

        onHoldButtonClicked: {
            callOverlayRect.overlayHoldButtonToggled()
        }

        onChatButtonClicked: {
            callOverlayRect.overlayChatButtonClicked()
        }

        onNoMicButtonClicked: {
            callOverlayRect.overlayNoMicButtonToggled()
        }

        onRecButtonClicked: {
            callOverlayRect.overlayRecButtonToggled()
        }

        onButtonEntered: {
            callOverlayRectMouseArea.entered()
        }

        states: [
            State {
                name: "entered"
                PropertyChanges {
                    target: callOverlayButtonGroup
                    opacity: 1
                }
            },
            State {
                name: "freezed"
                PropertyChanges {
                    target: callOverlayButtonGroup
                    opacity: 0
                }
            }
        ]

        transitions: Transition {
            PropertyAnimation {
                target: callOverlayButtonGroup
                property: "opacity"
                duration: 1000
            }
        }
    }

    MouseArea {
        id: callOverlayButtonGroupLeftSideMouseArea

        anchors.bottom: callOverlayRect.bottom
        anchors.left: callOverlayRect.left

        width: callOverlayRect.width / 6
        height: 60

        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        onEntered: {
            callOverlayRectMouseArea.entered()
        }

        onMouseXChanged: {
            callOverlayRectMouseArea.entered()
        }
    }

    MouseArea {
        id: callOverlayButtonGroupRightSideMouseArea

        anchors.bottom: callOverlayRect.bottom
        anchors.right: callOverlayRect.right

        width: callOverlayRect.width / 6
        height: 60

        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        onEntered: {
            callOverlayRectMouseArea.entered()
        }

        onMouseXChanged: {
            callOverlayRectMouseArea.entered()
        }
    }

    MouseArea {
        id: callOverlayRectMouseArea

        anchors.top: callOverlayRect.top
        anchors.topMargin: 50

        width: callOverlayRect.width
        height: callOverlayRect.height - callOverlayButtonGroup.height - 50

        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        onEntered: {
            if (overlayUpperPartRect.state !== 'entered') {
                overlayUpperPartRect.state = 'entered'
            }
            if (callOverlayButtonGroup.state !== 'entered') {
                callOverlayButtonGroup.state = 'entered'
            }
            callOverlayTimer.restart()
        }

        onMouseXChanged: {
            if (overlayUpperPartRect.state !== 'entered') {
                overlayUpperPartRect.state = 'entered'
            }
            if (callOverlayButtonGroup.state !== 'entered') {
                callOverlayButtonGroup.state = 'entered'
            }
            callOverlayTimer.restart()
        }
    }

    color: "transparent"
}