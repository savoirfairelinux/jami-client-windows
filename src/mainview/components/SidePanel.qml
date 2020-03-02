import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.jamitheme 1.0
import Qt.model.smartlist 1.0

import "../../commoncomponents"

Rectangle {
    id: sidePanelRect

    anchors.fill: parent

    AccountComboBox {
        id: accountComboBox

        anchors.top: sidePanelRect.top
        width: sidePanelRect.width
        height: 60
        currentIndex: 0

    }

    TabBar {
        id: tabBar

        anchors.top: accountComboBox.bottom
        anchors.topMargin: 20
        anchors.left: sidePanelRect.left
        anchors.leftMargin: tabBarLeftMargin

        width: sidePanelRect.width
        height: Math.max(pageOne.height, pageTwo.height)
        currentIndex: 0

        TabButton {
            id: pageOne

            width: tabBar.width / 2 - tabButtonShrinkSize
            height: textConvElement.height + 10
            down: true

            background: Rectangle {
                id: buttonRectOne

                radius: 10
                width: pageOne.width + 2
                color: pageOne.down ? "white" : JamiTheme.releaseColor
                border.color: JamiTheme.tabbarBorderColor

                Text {
                    id: textConvElement

                    anchors.centerIn: buttonRectOne

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: qsTr("Converstation")
                    font.pointSize: textFontSize
                    opacity: enabled ? 1.0 : 0.3
                }

                /*TextMetrics {
                    id:     textMetricsTextConvElement
                    font:   textConvElement.font
                    text:   qsTr("Converstation")
                }*/

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { buttonRectOne.color = JamiTheme.pressColor; tabBar.currentIndex = 0; pageOne.down = true; pageTwo.down = false;}
                    onReleased: { buttonRectOne.color = JamiTheme.releaseColor; }
                    onEntered: { buttonRectOne.color = JamiTheme.hoverColor; }
                    onExited: { buttonRectOne.color = Qt.binding(function() { return pageOne.down ? "white" : JamiTheme.releaseColor }); }
                }
            }
        }

        TabButton {
            id: pageTwo

            width: tabBar.width / 2 - tabButtonShrinkSize
            height: textInvElement.height + 10

            background: Rectangle {
                id: buttonRectTwo

                radius: 10
                color: pageTwo.down ? "white" : JamiTheme.releaseColor
                border.color: JamiTheme.tabbarBorderColor

                Text {
                    id: textInvElement

                    anchors.centerIn: buttonRectTwo

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    font.pointSize: textFontSize

                    text: qsTr("Invitation")
                    opacity: enabled ? 1.0 : 0.3
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { buttonRectTwo.color = JamiTheme.pressColor; tabBar.currentIndex = 1; pageTwo.down = true; pageOne.down = false;}
                    onReleased: { buttonRectTwo.color = JamiTheme.releaseColor; }
                    onEntered: { buttonRectTwo.color = JamiTheme.hoverColor; }
                    onExited: { buttonRectTwo.color = Qt.binding(function() { return pageTwo.down ? "white" : JamiTheme.releaseColor }); }
                }
            }
        }
    }

    Rectangle {
        id: stackLayoutRect

        anchors.top: tabBar.bottom
        anchors.topMargin: -12

        width: sidePanelRect.width
        height: sidePanelRect.height - accountComboBox.height - tabBar.height
        border.color: JamiTheme.tabbarBorderColor
        radius: 10

        Rectangle {
            id: hideTopBoarderRect

            anchors.top: stackLayoutRect.top
            anchors.left: stackLayoutRect.left
            anchors.leftMargin: tabBarLeftMargin + 5

            color: "white"
            width: pageOne.width + pageTwo.width - 9
            height: 1
        }

        StackLayout {
            id: stackLayoutView

            anchors.centerIn: stackLayoutRect
            width: stackLayoutRect.width - 10
            height: stackLayoutRect.height

            currentIndex: tabBar.currentIndex

            Item {
                id: stackOne

                anchors.fill: parent

                // search bar container to embed search label
                Rectangle {
                    id: contactSearchBarRect

                    anchors.top: parent.top
                    anchors.topMargin: 10
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 5

                    width: parent.width - 10
                    height: 35
                    border.color: JamiTheme.pressColor
                    radius: 10
                    color: contactSearchBar.activeFocus ? "white" : JamiTheme.hoverColor

                    Image {
                        id: searchIconImage

                        anchors.verticalCenter: contactSearchBarRect.verticalCenter
                        anchors.left: contactSearchBarRect.left

                        width: contactSearchBarRect.height
                        height: contactSearchBarRect.height
                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        source: "qrc:/images/icons/ic_baseline-search-24px.svg"
                    }

                    TextField {
                        id: contactSearchBar

                        anchors.verticalCenter: contactSearchBarRect.verticalCenter
                        anchors.left: searchIconImage.right

                        width: contactSearchBarRect.width - searchIconImage.width - 10
                        height: contactSearchBarRect.height - 5
                        font.pointSize: 8


                        Text {
                            id: placeholderTextForSearchBar

                            anchors.verticalCenter: contactSearchBar.verticalCenter
                            anchors.left: contactSearchBar.left
                            anchors.leftMargin: 5

                            text: qsTr("Find a new or existing contact")
                            font.pointSize: 8
                            color: "#aaa"
                            visible: !contactSearchBar.text && !contactSearchBar.activeFocus
                        }

                        background: Rectangle {
                            id: searchBarBackground

                            color: contactSearchBar.activeFocus ? "white" : JamiTheme.hoverColor
                       }
                        onTextChanged: {
                            mainViewWindow.searchBarTextChanged(contactSearchBar.text)
                        }
                    }
                }

                ListView {
                    id: conversationSmartListView

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    width: parent.width
                    height: parent.height -  contactSearchBarRect.height - 15
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

                        Text {
                            id: smartListUserName

                            anchors.left: conversationSmartListUserImage.right
                            anchors.leftMargin: 10
                            anchors.top: parent.top
                            fontSizeMode: Text.Fit

                            text: DisplayName
                            font.pointSize: 10
                        }

                        Text {
                            id: smartListUserSize

                            anchors.bottom: parent.bottom
                            anchors.left: conversationSmartListUserImage.right
                            anchors.leftMargin: 10
                            fontSizeMode: Text.Fit

                            text: size
                            font.pointSize: 10
                        }

                        background: Rectangle {
                            id: itemSmartListBackground

                            implicitWidth: conversationSmartListView.width
                            implicitHeight: smartListUserName.height + smartListUserSize.height + 10
                            border.width: 0
                        }

                        MouseArea {
                            anchors.fill: parent;
                            hoverEnabled: true;
                            acceptedButtons: Qt.LeftButton | Qt.RightButton

                            onPressed: { itemSmartListBackground.color = JamiTheme.pressColor; }
                            onReleased: {
                                itemSmartListBackground.color = JamiTheme.releaseColor
                                if (mouse.button === Qt.RightButton && Qt.platform.os == "windows") {
                                    // make menu pos at mouse
                                    var relativeMousePos = mapToItem(itemSmartListBackground, mouse.x, mouse.y)
                                    smartListContextMenu.x = relativeMousePos.x
                                    smartListContextMenu.y = relativeMousePos.y
                                    smartListContextMenu.open()
                                } else if (mouse.button === Qt.LeftButton && Qt.platform.os == "windows") {
                                    if(welcomeViewStack.visible)
                                        welcomeViewStack.push(popback)
                                    else
                                        callViewStack.push(popback)
                                    userTypeName = smartListUserName.text
                                }
                            }
                            onEntered: { itemSmartListBackground.color = JamiTheme.hoverColor; }
                            onExited: { itemSmartListBackground.color = "white"; }
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

                    ScrollBar.vertical: ScrollBar {
                        id: controlBar

                        parent: conversationSmartListView
                        anchors.top: conversationSmartListView.top
                        anchors.right: conversationSmartListView.right
                        size: 0.5
                        orientation: Qt.Vertical
                        policy: conversationSmartListView.contentHeight > conversationSmartListView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

                        contentItem: Rectangle {
                            implicitWidth: 5
                            implicitHeight: 100
                            radius: width / 2
                            color: controlBar.pressed ? "#81e889" : "#c2f4c6"
                        }
                        background: Rectangle {
                            color: JamiTheme.transparentColor
                        }
                    }
                }
            }

            Item {
                id: stackTwo

                anchors.fill: parent

                Image {
                    id: popImg

                    anchors.centerIn: parent
                    width: 50
                    height: 50
                    fillMode: Image.PreserveAspectFit
                    source: "images/jami.png"
                }
            }
        }
    }

    onWidthChanged: {
        /*if(tabBar.width != 0 && tabBar.width <= textMetricsTextConvElement.tightBoundingRect.width + tabButtonShrinkSize * 2 + pageTwo.width){
            console.log(tabBar.width)
            mainViewWindow.minWidth = splitView.width
        } else {
            mainViewWindow.minWidth = minWidth
        }*/
        //console.log(accountComboBox.width)
    }
}
