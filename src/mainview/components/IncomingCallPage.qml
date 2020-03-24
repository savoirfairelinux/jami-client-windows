import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Rectangle {
    id: incomingCallPageRect

    property int buttonPreferredSize: 50

    signal callAnswerButtonIsClicked()

    anchors.fill: parent

    ColumnLayout {
        id: incomingCallPageRectColumnLayout

        anchors.fill: parent

        Image {
            id: popImg

            Layout.alignment: Qt.AlignCenter

            Layout.preferredWidth: 100
            Layout.preferredHeight: 100

            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/jami.png"
        }

        Rectangle {
            id: incomingCallPageTextRect

            Layout.alignment: Qt.AlignCenter
            Layout.topMargin: 5

            Layout.preferredWidth: incomingCallPageRect.width
            Layout.preferredHeight: jamiBestNameText.height + jamiBestIdText.height + talkToYouText.height + 20

            ColumnLayout {
                id: incomingCallPageTextRectColumnLayout

                TextEdit {
                    id: jamiBestNameText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: incomingCallPageTextRect.width
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
                        text:   "bestName"
                        elideWidth: incomingCallPageTextRect.width
                        elide: Qt.ElideMiddle
                    }
                }

                TextEdit {
                    id: jamiBestIdText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: incomingCallPageTextRect.width
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
                        text:   "bestId"
                        elideWidth: incomingCallPageTextRect.width
                        elide: Qt.ElideMiddle
                    }
                }

                TextEdit {
                    id: talkToYouText

                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: incomingCallPageTextRect.width
                    Layout.preferredHeight: 30

                    selectByMouse: true
                    readOnly: true
                    font.pointSize: JamiTheme.textFontSize

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: "is calling you"
                }
            }
        }

        RowLayout {
            id: incomingCallPageRectRowLayout

            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: 20

            Layout.preferredWidth: incomingCallPageRect.width - 100
            Layout.preferredHeight: buttonPreferredSize

            HoverableButton {
                id: callAnswerButton

                Layout.alignment: Qt.AlignLeft

                Layout.preferredWidth: buttonPreferredSize
                Layout.preferredHeight: buttonPreferredSize

                backgroundColor: JamiTheme.acceptButtonGreen
                onEnterColor: JamiTheme.acceptButtonHoverGreen
                onPressColor: JamiTheme.acceptButtonPressedGreen
                onReleaseColor: JamiTheme.acceptButtonHoverGreen
                onExitColor: JamiTheme.acceptButtonGreen

                source: "qrc:/images/icons/ic_check_white_18dp_2x.png"
                radius: 30

                onClicked: {
                    incomingCallPageRect.callAnswerButtonIsClicked()
                }
            }

            HoverableButton {
                id: callDeclineButton

                Layout.alignment: Qt.AlignRight

                Layout.preferredWidth: buttonPreferredSize
                Layout.preferredHeight: buttonPreferredSize

                backgroundColor: JamiTheme.declineButtonRed
                onEnterColor: JamiTheme.declineButtonHoverRed
                onPressColor: JamiTheme.declineButtonPressedRed
                onReleaseColor: JamiTheme.declineButtonHoverRed
                onExitColor: JamiTheme.declineButtonRed

                source: "qrc:/images/icons/ic_close_white_24dp.png"
                radius: 30

                onClicked: {
                }
            }
        }
    }
}
