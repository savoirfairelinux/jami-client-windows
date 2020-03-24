import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Rectangle {
    id: callOverlayRect

    anchors.fill: parent

    Rectangle {
        id: overlayUpperPartRect

        anchors.top: callOverlayRect.top

        width: callOverlayRect.width
        height: 50

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
                PropertyChanges { target: overlayUpperPartRect; color: "red" }
            }
        ]
    }

    MouseArea {
        id: callOverlayRectMouseArea

        anchors.fill: parent

        hoverEnabled: true

        onEntered: {
            console.log("ssds")
            overlayUpperPartRect.state = 'entered'
        }
    }

    color: "transparent"
}
