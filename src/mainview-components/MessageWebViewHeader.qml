import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

Rectangle {
    id: messagingHeaderRect

    signal backToWelcomeViewButtonClicked()

    RowLayout {
        id: messagingHeaderRectRowLayout

        anchors.fill: parent

        Button {
            id: backToWelcomeViewButton

            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            Layout.leftMargin: 10
            Layout.preferredWidth: 30
            Layout.preferredHeight: 30

            background: Rectangle {
                id: backToWelcomeViewButtonBackground

                radius: 30

                Image {
                    id: backImage

                    anchors.centerIn: backToWelcomeViewButtonBackground

                    height: backToWelcomeViewButtonBackground.height - 10
                    width: backToWelcomeViewButtonBackground.width - 10

                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    source: "qrc:/images/icons/ic_arrow_back_24px.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { backToWelcomeViewButtonBackground.color = ColorConstant.pressColor; }
                    onReleased: { backToWelcomeViewButtonBackground.color = ColorConstant.releaseColor; messagingHeaderRect.backToWelcomeViewButtonClicked()}
                    onEntered: { backToWelcomeViewButtonBackground.color = ColorConstant.hoverColor; }
                    onExited: { backToWelcomeViewButtonBackground.color = "white"; }
                }
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

                    font.pointSize: textFontSize - 1

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsuserAliasLabel.elidedText
                }

                TextMetrics {
                    id:     textMetricsuserAliasLabel
                    font:   userAliasLabel.font
                    //
                    text:   accountListModel.data(accountListModel.index(0,0), 257)
                    elideWidth: userNameOrIdRect.width
                    elide: Qt.ElideMiddle
                }

                Label {
                    id: userUserNameLabel

                    Layout.alignment: Qt.AlignVCenter
                    Layout.preferredWidth: userNameOrIdRect.width
                    Layout.preferredHeight: textMetricsuserUserNameLabel.boundingRect.height

                    font.pointSize: textFontSize - 2
                    color: ColorConstant.faddedFontColor

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter

                    text: textMetricsuserUserNameLabel.elidedText
                }

                TextMetrics {
                    id:     textMetricsuserUserNameLabel
                    font:   userUserNameLabel.font
                    //
                    text:   accountListModel.data(accountListModel.index(0,0), 258)
                    elideWidth: userNameOrIdRect.width
                    elide: Qt.ElideMiddle
                }

            }
        }

        Rectangle {
            id: buttonGroup

            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.preferredWidth: 90
            Layout.preferredHeight: 30
            Layout.rightMargin: 20

            Button {
                id: startAAudioCallButton

                anchors.left: buttonGroup.left
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: 30
                width: 30

                background: Rectangle {
                    id: startAAudioCallButtonBackground

                    radius: 30

                    Image {
                        id: startAAudioCallButtonImage

                        anchors.centerIn: startAAudioCallButtonBackground

                        height: startAAudioCallButtonBackground.height - 10
                        width: startAAudioCallButtonBackground.width - 10

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        source: "qrc:/images/icons/ic_phone_24px.svg"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: { startAAudioCallButtonBackground.color = ColorConstant.pressColor; }
                        onReleased: { startAAudioCallButtonBackground.color = ColorConstant.releaseColor; }
                        onEntered: { startAAudioCallButtonBackground.color = ColorConstant.hoverColor; }
                        onExited: { startAAudioCallButtonBackground.color = "white"; }
                    }
                }
            }

            Button {
                id: startAVideoCallButton

                anchors.left: startAAudioCallButton.right
                anchors.leftMargin: 3
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: 30
                width: 30

                background: Rectangle {
                    id: startAVideoCallButtonBackground

                    radius: 30

                    Image {
                        id: startAVideoCallButtonImage

                        anchors.centerIn: startAVideoCallButtonBackground

                        height: startAVideoCallButtonBackground.height - 10
                        width: startAVideoCallButtonBackground.width - 10

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        source: "qrc:/images/icons/ic_video_call_24px.svg"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: { startAVideoCallButtonBackground.color = ColorConstant.pressColor; }
                        onReleased: { startAVideoCallButtonBackground.color = ColorConstant.releaseColor; }
                        onEntered: { startAVideoCallButtonBackground.color = ColorConstant.hoverColor; }
                        onExited: { startAVideoCallButtonBackground.color = "white"; }
                    }
                }
            }

            Button {
                id: sendContactRequestButton

                anchors.left: startAVideoCallButton.right
                anchors.leftMargin: 3
                anchors.verticalCenter: buttonGroup.verticalCenter

                height: 30
                width: 30

                background: Rectangle {
                    id: sendContactRequestButtonBackground

                    radius: 30

                    Image {
                        id: sendContactRequestButtonImage

                        anchors.centerIn: sendContactRequestButtonBackground

                        height: sendContactRequestButtonBackground.height - 10
                        width: sendContactRequestButtonBackground.width - 10

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        source: "qrc:/images/icons/ic_person_add_black_24dp_2x.png"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: { sendContactRequestButtonBackground.color = ColorConstant.pressColor; }
                        onReleased: { sendContactRequestButtonBackground.color = ColorConstant.releaseColor; }
                        onEntered: { sendContactRequestButtonBackground.color = ColorConstant.hoverColor; }
                        onExited: { sendContactRequestButtonBackground.color = "white"; }
                    }
                }
            }
        }
    }

    onWidthChanged: {

    }
}
