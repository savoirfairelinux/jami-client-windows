import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0

ListView {
    id: conversationSmartListView

    signal needToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias, string currentUID, bool callStackViewShouldShow)
    signal needToDeselectItems
    signal needToBackToWelcomePage

    signal currentIndexIsChanged

    function modelSorted(contactURIToCompare) {
        var conversationSmartListViewModel = conversationSmartListView.model
        conversationSmartListView.currentIndex = -1
        updateConversationSmartListView()
        for (var i = 0; i < count; i++) {
            if (conversationSmartListViewModel.data(
                        conversationSmartListViewModel.index(i, 0),
                        261) === contactURIToCompare) {
                conversationSmartListView.currentIndex = i
                break
            }
        }
    }

    function updateConversationSmartListView() {
        var conversationSmartListViewModel = conversationSmartListView.model
        conversationSmartListViewModel.dataChanged(
                    conversationSmartListViewModel.index(0, 0),
                    conversationSmartListViewModel.index(
                        conversationSmartListViewModel.rowCount() - 1, 0))
    }

    function setModel(model) {
        conversationSmartListView.model = model
    }

    function backToWelcomePage() {
        conversationSmartListView.needToBackToWelcomePage()
    }

    function updateSmartList(accountId) {
        conversationSmartListView.model.setAccount(accountId)
    }

    Connections {
        target: CallCenter

        onNeedToUpdateConversationSmartList: {
            updateConversationSmartListView()
        }
    }

    onCurrentIndexChanged: {
        conversationSmartListView.currentIndexIsChanged()
    }

    clip: true

    delegate: ItemDelegate {
        id: smartListItemDelegate

        Connections {
            target: conversationSmartListView

            onCurrentIndexIsChanged: {
                if (conversationSmartListView.currentIndex === -1
                        || conversationSmartListView.currentIndex !== index) {
                    itemSmartListBackground.color = Qt.binding(function () {
                        return InCall ? Qt.lighter(JamiTheme.selectionBlue,
                                                   1.8) : "white"
                    })
                } else {
                    itemSmartListBackground.color = Qt.binding(function () {
                        return InCall ? Qt.lighter(JamiTheme.selectionBlue,
                                                   1.8) : JamiTheme.releaseColor
                    })
                }
            }
        }

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

            Rectangle {
                id: presenseRect

                anchors.right: conversationSmartListUserImage.right
                anchors.rightMargin: 1
                anchors.bottom: conversationSmartListUserImage.bottom
                anchors.bottomMargin: 2

                width: 14
                height: 14

                // enum REGISTERED == 5
                visible: Presence

                Rectangle {
                    id: presenseCycle

                    anchors.centerIn: presenseRect

                    width: 10
                    height: 10

                    radius: 30
                    color: JamiTheme.presenceGreen
                }

                radius: 30
                color: "white"
            }

            Rectangle {
                id: unreadMessageCountRect

                anchors.right: conversationSmartListUserImage.right
                anchors.rightMargin: 1
                anchors.top: conversationSmartListUserImage.top
                anchors.topMargin: 2

                width: 14
                height: 14

                visible: UnreadMessagesCount > 0

                Text {
                    id: unreadMessageCounttext

                    anchors.centerIn: unreadMessageCountRect

                    text: UnreadMessagesCount > 9 ? "···" : UnreadMessagesCount
                    color: "white"
                }

                radius: 30
                color: JamiTheme.notificationRed
            }
        }

        Rectangle {
            id: conversationSmartListUserInfoRect

            anchors.left: conversationSmartListUserImage.right
            anchors.leftMargin: 10
            anchors.top: parent.top

            width: (parent.width - conversationSmartListUserImage.width - 20) / 2
            height: parent.height

            color: "transparent"

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
                    font.pointSize: JamiTheme.textFontSize
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
                    font.pointSize: JamiTheme.textFontSize
                }
            }
        }

        Rectangle {
            id: conversationSmartListUserLastInteractionRect

            anchors.left: conversationSmartListUserInfoRect.right
            anchors.top: parent.top

            width: (parent.width - conversationSmartListUserImage.width - 20) / 2 - 10
            height: parent.height

            color: "transparent"

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
                    font.pointSize: JamiTheme.textFontSize
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
                        text: InCall ? CallStateStr : LastInteraction
                    }

                    text: textMetricsConversationSmartListUserLastInteractionMessage.elidedText
                    font.pointSize: JamiTheme.textFontSize
                    color: JamiTheme.faddedLastInteractionFontColor
                }
            }
        }

        background: Rectangle {
            id: itemSmartListBackground

            color: InCall ? Qt.lighter(JamiTheme.selectionBlue, 1.8) : "white"

            implicitWidth: conversationSmartListView.width
            implicitHeight: Math.max(
                                conversationSmartListUserName.height
                                + textMetricsConversationSmartListUserId.height + 10,
                                conversationSmartListUserImage.height + 10)
            border.width: 0
        }

        MouseArea {
            id: mouseAreaSmartListItemDelegate

            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: {
                if (!InCall) {
                    itemSmartListBackground.color = JamiTheme.pressColor
                }
            }
            onReleased: {
                conversationSmartListView.currentIndex = index
                if (!InCall) {
                    itemSmartListBackground.color = JamiTheme.releaseColor
                }
                if (mouse.button === Qt.RightButton
                        && Qt.platform.os == "windows") {
                    // make menu pos at mouse
                    var relativeMousePos = mapToItem(itemSmartListBackground,
                                                     mouse.x, mouse.y)
                    smartListContextMenu.x = relativeMousePos.x
                    smartListContextMenu.y = relativeMousePos.y
                    smartListContextMenu.open()
                } else if (mouse.button === Qt.LeftButton
                           && Qt.platform.os == "windows") {
                    conversationSmartListView.needToAccessMessageWebView(
                                DisplayID == DisplayName ? "" : DisplayID,
                                DisplayName, UID, CallStackViewShouldShow)
                }
            }
            onEntered: {
                if (!InCall) {
                    itemSmartListBackground.color = JamiTheme.hoverColor
                }
            }
            onExited: {
                if (!InCall) {
                    if (conversationSmartListView.currentIndex != index
                            || conversationSmartListView.currentIndex == -1) {
                        itemSmartListBackground.color = Qt.binding(function () {
                            return InCall ? Qt.lighter(JamiTheme.selectionBlue,
                                                       1.8) : "white"
                        })
                    } else {
                        itemSmartListBackground.color = Qt.binding(function () {
                            return InCall ? Qt.lighter(
                                                JamiTheme.selectionBlue,
                                                1.8) : JamiTheme.releaseColor
                        })
                    }
                }
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
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: {
                            smartListContextMenuBackRect.color = JamiTheme.pressColor
                        }
                        onReleased: {
                            smartListContextMenuBackRect.color = JamiTheme.releaseColor
                            smartListContextMenu.close()
                            contextMenuPopUpMock.open()
                        }
                        onEntered: {
                            smartListContextMenuBackRect.color = JamiTheme.hoverColor
                        }
                        onExited: {
                            smartListContextMenuBackRect.color = "white"
                        }
                    }
                }
            }
            background: Rectangle {
                implicitWidth: 150
                implicitHeight: 30
            }
        }
    }

    ScrollIndicator.vertical: ScrollIndicator {}
}
