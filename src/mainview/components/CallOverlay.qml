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

    anchors.fill: parent

    Timer {
        id: callOverlayTimer
        interval: 5000
        onTriggered: {
            if(overlayUpperPartRect.state !== 'freezed') {
                overlayUpperPartRect.state = 'freezed'
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
                    id:     textMetricsjamiBestNameText
                    font:   jamiBestNameText.font
                    text:   "bestName"
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
                    id:     textMetricscallTimerText
                    font:   callTimerText.font
                    text:   "00:00"
                    elideWidth: overlayUpperPartRect.width / 3
                    elide: Qt.ElideMiddle
                }
            }
        }

        color: "transparent"

        states: [
            State {
                name: "entered"
                PropertyChanges { target: overlayUpperPartRect; opacity: 1 }
            },
            State {
                name: "freezed"
                PropertyChanges { target: overlayUpperPartRect; opacity: 0 }
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

    Rectangle {
        id: overlayBottomPartRect

        anchors.bottom: callOverlayRect.bottom

        width: callOverlayRect.width
        height: 50
        opacity: 1

        RowLayout {
            id: overlayBottomPartRectRowLayout

            anchors.fill: parent

            TintedButton {
                id: hangUpButton

                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.leftMargin: 20
                Layout.preferredHeight: 30
                Layout.preferredWidth : 30

                tintColor: JamiTheme.hangUpButtonTintedRed
                normalPixmapSource: "qrc:/images/icons/ic_close_white_24dp.png"
                selectedPixmapSource: "qrc:/images/icons/ic_call_transfer_white_24px.png"

                onButtonEntered: {
                    callOverlayRectMouseArea.entered()
                }
            }
        }

        color: "transparent"
    }

    MouseArea {
        id: callOverlayRectMouseArea

        anchors.top: callOverlayRect.top

        width: callOverlayRect.width
        height: callOverlayRect.height - overlayBottomPartRect.height

        hoverEnabled: true
        propagateComposedEvents: true
        acceptedButtons: Qt.NoButton

        onEntered: {
            if(overlayUpperPartRect.state !== 'entered') {
                overlayUpperPartRect.state = 'entered'
                callOverlayTimer.restart()
            }
        }

        onMouseXChanged: {
            if(overlayUpperPartRect.state !== 'entered') {
                overlayUpperPartRect.state = 'entered'
                callOverlayTimer.restart()
            }
        }

        onMouseYChanged: {
            if(overlayUpperPartRect.state !== 'entered') {
                overlayUpperPartRect.state = 'entered'
                callOverlayTimer.restart()
            }
        }
    }

    color: "transparent"
}
