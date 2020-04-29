import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import net.jami.JamiTheme 1.0
import net.jami.Callcenter 1.0

import "../../commoncomponents"

Rectangle {
    id: outgoingCallPageRect

    property int buttonPreferredSize: 50
    property string callStatusPresentation: "outgoing"
    property string contactImgSource: ""
    property string bestName: "Best Name"
    property string bestId: "Best Id"

    signal callCancelButtonIsClicked
    signal backButtonIsClicked

    function updateUI(accountId, convUid) {
        contactImgSource = "data:image/png;base64," + utilsAdapter.getContactImageString(
                    accountId, convUid)
        bestName = utilsAdapter.getBestName(accountId, convUid)
        var id = utilsAdapter.getBestId(accountId, convUid)
        bestId = (bestName !== id) ? id : ""
    }

    anchors.fill: parent

    color: "black"

    TintedButton {
        id: backTintedButton

        anchors.top: outgoingCallPageRect.top
        anchors.topMargin: 10
        anchors.left: outgoingCallPageRect.left
        anchors.leftMargin: 5

        width: 30
        height: 30

        tintColor: JamiTheme.buttonTintedBlue
        normalPixmapSource: "qrc:/images/icons/ic_arrow_back_white_24dp.png"
        selectedPixmapSource: "qrc:/images/icons/ic_arrow_back_white_24dp.png"

        onClicked: {
            outgoingCallPageRect.backButtonIsClicked()
        }
    }

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
            Layout.preferredHeight: jamiBestNameText.height + jamiBestIdText.height
                                    + callStatusText.height + spinnerImage.height + 20

            color: "transparent"

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
                    color: "white"

                    TextMetrics {
                        id: textMetricsjamiBestNameText
                        font: jamiBestNameText.font
                        text: bestName
                        elideWidth: outgoingCallPageTextRect.width - 50
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
                    color: Qt.lighter("white", 1.5)

                    TextMetrics {
                        id: textMetricsjamiBestIdText
                        font: jamiBestIdText.font
                        text: bestId
                        elideWidth: outgoingCallPageTextRect.width - 50
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
                    color: Qt.lighter("white", 1.5)
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
                    outgoingCallPageRect.callCancelButtonIsClicked()
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
