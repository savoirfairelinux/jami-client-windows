import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Rectangle {
    id: messagingHeaderRect

    property int buttonPreferredSize: 30
    property string userAliasLabelText: ""
    property string userUserNameLabelText: ""
    property bool sendContactRequestButtonVisible: true

    signal backToWelcomeViewButtonClicked()

    RowLayout {
        id: messagingHeaderRectRowLayout

        anchors.fill: parent

        HoverableButton {
            id: backToWelcomeViewButton

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.leftMargin: 10
            Layout.preferredWidth: buttonPreferredSize
            Layout.preferredHeight: buttonPreferredSize

            radius: 30
            source: "qrc:/images/icons/ic_arrow_back_24px.svg"
            backgroundColor: "white"
            onExitColor: "white"

            onClicked: {
                messagingHeaderRect.backToWelcomeViewButtonClicked()
            }
        }

        Rectangle {
            id: userNameOrIdRect

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            // width + margin
            Layout.preferredWidth: Math.max(100, messagingHeaderRect.width - backToWelcomeViewButton.width - buttonGroup.width - 30 - 6 - 5)
            Layout.preferredHeight: messagingHeaderRect.height
            Layout.leftMargin: 5

            ColumnLayout {
                id: userNameOrIdColumnLayout

                Label {
                    id: userAliasLabel

                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: userNameOrIdRect.width
                    Layout.preferredHeight: textMetricsuserAliasLabel.boundingRect.height
                    Layout.topMargin: userNameOrIdRect.height / 2 - userAliasLabel.height - 4

                    font.pointSize: JamiTheme.textFontSize - 1

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsuserAliasLabel.elidedText
                }

                TextMetrics {
                    id:     textMetricsuserAliasLabel
                    font:   userAliasLabel.font
                    //
                    text:   userAliasLabelText
                    elideWidth: userNameOrIdRect.width
                    elide: Qt.ElideMiddle
                }

                Label {
                    id: userUserNameLabel

                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: userNameOrIdRect.width
                    Layout.preferredHeight: textMetricsuserUserNameLabel.boundingRect.height

                    font.pointSize: JamiTheme.textFontSize - 2
                    color: JamiTheme.faddedFontColor

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsuserUserNameLabel.elidedText
                }

                TextMetrics {
                    id:     textMetricsuserUserNameLabel
                    font:   userUserNameLabel.font
                    //
                    text:   userUserNameLabelText
                    elideWidth: userNameOrIdRect.width
                    elide: Qt.ElideMiddle
                }

            }
        }

        Rectangle {
            id: buttonGroup

            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.preferredWidth: buttonPreferredSize * 3
            Layout.preferredHeight: buttonPreferredSize
            Layout.rightMargin: 20

            HoverableButton {
                id: startAAudioCallButton

                anchors.left: buttonGroup.left
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: buttonPreferredSize
                width: buttonPreferredSize

                radius: 30
                source: "qrc:/images/icons/ic_phone_24px.svg"
                backgroundColor: "white"
                onExitColor: "white"

                onClicked: {
                    var component = Qt.createComponent("CallStackViewWindow.qml")
                    var window    = component.createObject()
                    window.show()
                }
            }

            HoverableButton {
                id: startAVideoCallButton

                anchors.left: startAAudioCallButton.right
                anchors.leftMargin: 3
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: buttonPreferredSize
                width: buttonPreferredSize

                radius: 30
                source: "qrc:/images/icons/ic_video_call_24px.svg"
                backgroundColor: "white"
                onExitColor: "white"
            }

            HoverableButton {
                id: sendContactRequestButton

                anchors.left: startAVideoCallButton.right
                anchors.leftMargin: 3
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: buttonPreferredSize
                width: buttonPreferredSize

                visible: sendContactRequestButtonVisible
                radius: 30
                source: "qrc:/images/icons/ic_person_add_black_24dp_2x.png"
                backgroundColor: "white"
                onExitColor: "white"
            }
        }
    }
}
