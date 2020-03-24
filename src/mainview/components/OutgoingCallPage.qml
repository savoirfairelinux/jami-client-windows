import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0
import net.jami.callcenter 1.0

import "../../commoncomponents"

Rectangle {
    id: outgoingCallPageRect

    property int buttonPreferredSize: 50
    property string callStatusPresentation: "outgoing"
    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    signal callAnswerButtonIsClicked()

    anchors.fill: parent

    ColumnLayout {
        id: outgoingCallPageRectColumnLayout

        anchors.fill: parent

        Image {
            id: contactImg

            Layout.alignment: Qt.AlignCenter

            Layout.preferredWidth: 100
            Layout.preferredHeight: 100

            fillMode: Image.PreserveAspectFit
            source: contactImgSource
            asynchronous: true
        }

        Rectangle {
            id: outgoingCallPageTextRect

            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: 5

            Layout.preferredWidth: outgoingCallPageRect.width
            Layout.preferredHeight: jamiBestNameText.height + jamiBestIdText.height + callStatusText.height + spinnerImage.height + 20

            ColumnLayout {
                id: outgoingCallPageTextRectColumnLayout

                TextEdit {
                    id: jamiBestNameText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: outgoingCallPageTextRect.width
                    Layout.preferredHeight: 50

                    selectByMouse: true
                    readOnly: true
                    font.pointSize: JamiTheme.textFontSize + 3

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsjamiBestNameText.elidedText

                    TextMetrics {
                        id:     textMetricsjamiBestNameText
                        font:   jamiBestNameText.font
                        text:   bestName
                        elideWidth: outgoingCallPageTextRect.width
                        elide: Qt.ElideMiddle
                    }
                }

                TextEdit {
                    id: jamiBestIdText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: outgoingCallPageTextRect.width
                    Layout.preferredHeight: 30

                    selectByMouse: true
                    readOnly: true
                    font.pointSize: JamiTheme.textFontSize

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsjamiBestIdText.elidedText
                    color: JamiTheme.faddedFontColor

                    TextMetrics {
                        id:     textMetricsjamiBestIdText
                        font:   jamiBestIdText.font
                        text:   bestId
                        elideWidth: outgoingCallPageTextRect.width
                        elide: Qt.ElideMiddle
                    }
                }

                AnimatedImage {
                    id: spinnerImage

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: 20
                    Layout.preferredHeight: 5

                    source: "qrc:/images/waiting.gif"
                }

                TextEdit {
                    id: callStatusText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: outgoingCallPageTextRect.width
                    Layout.preferredHeight: 30

                    selectByMouse: true
                    readOnly: true
                    font.pointSize: JamiTheme.textFontSize

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: callStatusPresentation + "..."
                    color: JamiTheme.faddedFontColor
                }
            }
        }

        ColumnLayout {
            id: callCancelButtonColumnLayout

            Layout.alignment: Qt.AlignCenter

            HoverableButton {
                id: callCancelButton

                Layout.alignment: Qt.AlignCenter

                Layout.preferredWidth: buttonPreferredSize
                Layout.preferredHeight: buttonPreferredSize

                backgroundColor: JamiTheme.declineButtonRed
                onEnterColor: JamiTheme.declineButtonHoverRed
                onPressColor: JamiTheme.declineButtonPressedRed
                onReleaseColor: JamiTheme.declineButtonHoverRed
                onExitColor: JamiTheme.declineButtonRed

                buttonImageHeight: buttonPreferredSize / 2
                buttonImageWidth: buttonPreferredSize / 2
                source: "qrc:/images/icons/ic_close_white_24dp.png"
                radius: 30

                onClicked: {
                }
            }

            Text {
                id: cancelText

                Layout.alignment: Qt.AlignCenter

                font.pointSize: JamiTheme.textFontSize - 2
                text: qsTr("Cancel")
            }
        }
    }
}
