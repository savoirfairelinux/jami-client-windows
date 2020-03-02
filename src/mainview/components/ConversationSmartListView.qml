import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0
import net.jami.model.smartlist 1.0

ListView {
    id: conversationSmartListView

    signal needToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias)

    model: ConversationSmartListModel
    clip: true

    delegate: ItemDelegate {

        Image {
            id: conversationSmartListUserImage

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10

            width: 50
            height: 50

            fillMode: Image.PreserveAspectFit
            source: "data:image/png;base64," + Picture
            mipmap: true
        }

        Rectangle {
            id: conversationSmartListUserInfoRect

            anchors.left: conversationSmartListUserImage.right
            anchors.leftMargin: 10
            anchors.top: parent.top

            width: (parent.width - conversationSmartListUserImage.width - 20) / 2
            height: parent.height

            ColumnLayout {
                id: conversationSmartListUserInfoColumnLayout

                anchors.fill: parent

                Text {
                    id: conversationSmartListUserName

                    Layout.alignment: Qt.AlignLeft

                    TextMetrics {
                        id: textMetricsConversationSmartListUserName
                        font: conversationSmartListUserName.font
                        elide: Text.ElideMiddle
                        elideWidth: conversationSmartListUserInfoRect.width
                        text: DisplayName
                    }

                    text: textMetricsConversationSmartListUserName.elidedText
                    font.pointSize: textFontSize
                }

                Text {
                    id: conversationSmartListUserId

                    Layout.alignment: Qt.AlignLeft

                    fontSizeMode: Text.Fit
                    color: JamiTheme.faddedFontColor

                    TextMetrics {
                        id: textMetricsConversationSmartListUserId
                        font: conversationSmartListUserId.font
                        elide: Text.ElideMiddle
                        elideWidth: conversationSmartListUserInfoRect.width
                        text: DisplayID == DisplayName ? "" : DisplayID
                    }

                    text: textMetricsConversationSmartListUserId.elidedText
                    font.pointSize: textFontSize
                }
            }
        }

        Rectangle {
            id: conversationSmartListUserLastInteractionRect

            anchors.left: conversationSmartListUserInfoRect.right
            anchors.top: parent.top

            width: (parent.width - conversationSmartListUserImage.width - 20) / 2 - 10
            height: parent.height

            ColumnLayout {
                id: conversationSmartListUserLastInteractionColumnLayout

                anchors.fill: parent

                Text {
                    id: conversationSmartListUserLastInteractionDate

                    Layout.alignment: Qt.AlignRight

                    TextMetrics {
                        id: textMetricsConversationSmartListUserLastInteractionDate
                        font: conversationSmartListUserLastInteractionDate.font
                        elide: Text.ElideMiddle
                        elideWidth: conversationSmartListUserLastInteractionRect.width
                        text: LastInteractionDate
                    }

                    text: textMetricsConversationSmartListUserLastInteractionDate.elidedText
                    font.pointSize: textFontSize
                    color: JamiTheme.faddedFontColor
                }

                Text {
                    id: conversationSmartListUserLastInteractionMessage

                    Layout.alignment: Qt.AlignRight

                    fontSizeMode: Text.Fit

                    TextMetrics {
                        id: textMetricsConversationSmartListUserLastInteractionMessage
                        font: conversationSmartListUserLastInteractionMessage.font
                        elide: Text.ElideMiddle
                        elideWidth: conversationSmartListUserLastInteractionRect.width
                        text: LastInteraction
                    }

                    text: textMetricsConversationSmartListUserLastInteractionMessage.elidedText
                    font.pointSize: textFontSize
                    color: JamiTheme.faddedLastInteractionFontColor
                }
            }
        }

        background: Rectangle {
            id: itemSmartListBackground

            implicitWidth: conversationSmartListView.width
            implicitHeight: Math.max(conversationSmartListUserName.height + textMetricsConversationSmartListUserId.height + 10, conversationSmartListUserImage.height + 10)
            border.width: 0
        }

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: {
                itemSmartListBackground.color = JamiTheme.pressColor
                conversationSmartListUserLastInteractionRect.color = JamiTheme.pressColor
                conversationSmartListUserInfoRect.color = JamiTheme.pressColor
            }
            onReleased: {
                itemSmartListBackground.color = JamiTheme.releaseColor
                conversationSmartListUserLastInteractionRect.color = JamiTheme.releaseColor
                conversationSmartListUserInfoRect.color = JamiTheme.releaseColor
                if (mouse.button === Qt.RightButton && Qt.platform.os == "windows") {
                    // make menu pos at mouse
                    var relativeMousePos = mapToItem(itemSmartListBackground, mouse.x, mouse.y)
                    smartListContextMenu.x = relativeMousePos.x
                    smartListContextMenu.y = relativeMousePos.y
                    smartListContextMenu.open()
                } else if (mouse.button === Qt.LeftButton && Qt.platform.os == "windows") {
                    conversationSmartListView.needToAccessMessageWebView(DisplayID == DisplayName ? "" : DisplayID, DisplayName)
                }
            }
            onEntered: {
                itemSmartListBackground.color = JamiTheme.hoverColor
                conversationSmartListUserLastInteractionRect.color = JamiTheme.hoverColor
                conversationSmartListUserInfoRect.color = JamiTheme.hoverColor
            }
            onExited: {
                itemSmartListBackground.color = "white"
                conversationSmartListUserLastInteractionRect.color = "white"
                conversationSmartListUserInfoRect.color = "white"
            }
        }
        Menu {
            id: smartListContextMenu
            MenuItem {
                id: smartListCopyItem
                text: qsTr("Popup")
                font.pointSize: 10
                background: Rectangle {
                    id: smartListContextMenuBackRect

                    implicitWidth: 150
                    implicitHeight: 30
                    border.width: 1
                    border.color: "black"
                    color: smartListCopyItem.down ? JamiTheme.releaseColor : "white"
                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onPressed: { smartListContextMenuBackRect.color = JamiTheme.pressColor; }
                        onReleased: {
                            smartListContextMenuBackRect.color = JamiTheme.releaseColor
                            smartListContextMenu.close()
                            contextMenuPopUpMock.open()
                        }
                        onEntered: { smartListContextMenuBackRect.color = JamiTheme.hoverColor; }
                        onExited: { smartListContextMenuBackRect.color = "white"; }
                    }
                }
            }
            background: Rectangle {
                implicitWidth: 150
                implicitHeight: 30
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator { }
}
