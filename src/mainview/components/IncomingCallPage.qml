import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.tools.utils 1.0
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Window {
    id: incomingCallPage

    property int minWidth: 300
    property int minHeight: 400

    property string responsibleAccountId: ""
    property string responsibleConvUid: ""

    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    property int buttonPreferredSize: 50
    property variant clickPos: "1,1"

    signal callAnswerButtonIsClicked()

    function updateUI() {
        incomingCallPage.contactImgSource = "data:image/png;base64," + utilsAdapter.getContactImageString(responsibleAccountId, responsibleConvUid)
        incomingCallPage.bestName = utilsAdapter.getBestName(responsibleAccountId, responsibleConvUid)
        var id = utilsAdapter.getBestId(responsibleAccountId, responsibleConvUid)
        incomingCallPage.bestId = (incomingCallPage.bestName !== id) ? id : ""
    }

    minimumWidth: minWidth
    minimumHeight: minHeight

    maximumWidth: minWidth + 300
    maximumHeight: minHeight + 300

    x: screen.virtualX + screen.width - width
    y: screen.virtualY + screen.height - height - 50

    flags: Qt.SubWindow | Qt.Tool | Qt.FramelessWindowHint | Qt.WindowSystemMenuHint | Qt.WindowStaysOnTopHint
    screen: Qt.application.screens[0]

    UtilsAdapter {
        id: utilsAdapter
    }

    Rectangle {
        id: incomingCallPageColumnLayoutMainRect

        anchors.fill: parent

        radius: 15
        color: "black"

        MouseArea {
            id: dragMouseArea

            anchors.left: incomingCallPageColumnLayoutMainRect.left
            anchors.top: incomingCallPageColumnLayoutMainRect.top

            width: incomingCallPageColumnLayoutMainRect.width - closeButton.width - 10
            height: 30

            onPressed: {
                clickPos = Qt.point(mouse.x,mouse.y)
            }

            onPositionChanged: {
                var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                incomingCallPage.x += delta.x;
                incomingCallPage.y += delta.y;
            }
        }

        HoverableButton {
            id: closeButton

            anchors.top: incomingCallPageColumnLayoutMainRect.top
            anchors.topMargin: 10
            anchors.right: incomingCallPageColumnLayoutMainRect.right
            anchors.rightMargin: 10

            width: 30
            height: 30

            radius: 30
            source: "qrc:/images/icons/ic_close_white_24dp.png"
            backgroundColor: "black"
            onEnterColor: "#4c4c4c"
            onExitColor: "black"
            onPressColor: JamiTheme.declineButtonPressedRed
            onReleaseColor: "black"

            onClicked: {
                incomingCallPage.close()
                CallCenter.refuseACall(responsibleAccountId, responsibleConvUid)
            }
        }

        ColumnLayout {
            id: incomingCallPageColumnLayout

            anchors.fill: parent

            Image {
                id: contactImg

                Layout.alignment: Qt.AlignCenter
                Layout.topMargin: 30

                Layout.preferredWidth: 100
                Layout.preferredHeight: 100

                fillMode: Image.PreserveAspectFit
                source: contactImgSource
            }

            Rectangle {
                id: incomingCallPageTextRect

                Layout.alignment: Qt.AlignCenter
                Layout.topMargin: 5

                Layout.preferredWidth: incomingCallPage.width
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
                        color: "white"

                        TextMetrics {
                            id:     textMetricsjamiBestNameText
                            font:   jamiBestNameText.font
                            text:   bestName
                            elideWidth: incomingCallPageTextRect.width - 30
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
                        color: "white"

                        TextMetrics {
                            id:     textMetricsjamiBestIdText
                            font:   jamiBestIdText.font
                            text:   bestId
                            elideWidth: incomingCallPageTextRect.width - 30
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
                        color: "white"

                        text: "is calling you"
                    }
                }

                color: "transparent"
            }

            RowLayout {
                id: incomingCallPageRowLayout

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: 5

                Layout.preferredWidth: incomingCallPage.width - 100
                Layout.preferredHeight: buttonPreferredSize

                ColumnLayout {
                    id: callAnswerButtonColumnLayout

                    Layout.alignment: Qt.AlignLeft

                    HoverableButton {
                        id: callAnswerButton

                        Layout.alignment: Qt.AlignCenter

                        Layout.preferredWidth: buttonPreferredSize
                        Layout.preferredHeight: buttonPreferredSize

                        backgroundColor: JamiTheme.acceptButtonGreen
                        onEnterColor: JamiTheme.acceptButtonHoverGreen
                        onPressColor: JamiTheme.acceptButtonPressedGreen
                        onReleaseColor: JamiTheme.acceptButtonHoverGreen
                        onExitColor: JamiTheme.acceptButtonGreen

                        buttonImageHeight: buttonPreferredSize / 2
                        buttonImageWidth: buttonPreferredSize / 2
                        source: "qrc:/images/icons/ic_check_white_18dp_2x.png"
                        radius: 30

                        onClicked: {
                            incomingCallPage.callAnswerButtonIsClicked()
                        }
                    }

                    Text {
                        id: answerText

                        Layout.alignment: Qt.AlignCenter

                        font.pointSize: JamiTheme.textFontSize - 2
                        text: qsTr("Answer")
                    }
                }

                ColumnLayout {
                    id: callDeclineButtonColumnLayout

                    Layout.alignment: Qt.AlignRight

                    HoverableButton {
                        id: callDeclineButton

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
                            incomingCallPage.close()
                            CallCenter.refuseACall(responsibleAccountId, responsibleConvUid)
                        }
                    }

                    Text {
                        id: ignoreText

                        Layout.alignment: Qt.AlignCenter

                        font.pointSize: JamiTheme.textFontSize - 2
                        text: qsTr("Ignore")
                    }
                }
            }
        }
    }

    color: "transparent"
}
