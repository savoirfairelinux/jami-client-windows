import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

import "../qmlcomponentutils"

Rectangle {
    id: sidePanelRect

    anchors.fill: parent

    ComboBox {
        id: accountComboBox

        anchors.top: sidePanelRect.top
        width: sidePanelRect.width
        height: 60
        currentIndex: 0

        Image {
            id: userImageRoot

            anchors.left: accountComboBox.left
            anchors.leftMargin: 5
            anchors.verticalCenter: accountComboBox.verticalCenter


            width: accountComboBox.height - 10
            height: accountComboBox.height - 10
            fillMode: Image.PreserveAspectFit
            source: "image://accountImage/account_0_0"
            mipmap: true
        }

        Text {
            id: textUserAliasRoot

            anchors.left: userImageRoot.right
            anchors.leftMargin: 10
            anchors.top: rootItemBackground.top
            anchors.topMargin: 5

            text: textMetricsUserAliasRoot.elidedText
            font.pointSize: textFontSize
        }

        Text {
            id: textUsernameRoot

            anchors.left: userImageRoot.right
            anchors.leftMargin: 10
            anchors.top: textUserAliasRoot.bottom
            anchors.topMargin: 5

            text: textMetricsUsernameRoot.elidedText
            font.pointSize: textFontSize
            color: ColorConstant.faddedFontColor
        }

        TextMetrics {
            id: textMetricsUserAliasRoot
            font: textUserAliasRoot.font
            elide: Text.ElideMiddle
            elideWidth: accountComboBox.width - userImageRoot.width - settingsButton.width - 25
            text: accountListModel.data(accountListModel.index(0,0), 257)
        }

        TextMetrics {
            id: textMetricsUsernameRoot
            font: textUsernameRoot.font
            elide: Text.ElideMiddle
            elideWidth: accountComboBox.width - userImageRoot.width - settingsButton.width - 25
            text: accountListModel.data(accountListModel.index(0,0), 258)
        }

        Button {
            id: settingsButton

            anchors.right: accountComboBox.right
            anchors.rightMargin: 10
            anchors.verticalCenter: accountComboBox.verticalCenter
            width: 20
            height: 20

            background: Rectangle {
                id: settingButtonRect

                radius: 30
                Image {
                    id: settingsButtonImage

                    anchors.centerIn: settingButtonRect
                    width: settingsButton.width - 2
                    height: settingsButton.height - 2
                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    source: "qrc:/images/icons/round-settings-24px.svg"
                }
            }
        }

        background: Rectangle {
            id: rootItemBackground

            implicitWidth: accountComboBox.width
            implicitHeight: accountComboBox.height
            border.width: 0
        }

        MouseArea {
            id: comboBoxRootMouseArea
            anchors.fill: parent
            hoverEnabled: true
            propagateComposedEvents: true
            onPressed: {
                if(isMouseOnSettingsButton(mouse))
                    settingButtonRect.color = ColorConstant.pressColor
                else
                    rootItemBackground.color = ColorConstant.pressColor
            }
            onReleased: {
                if(isMouseOnSettingsButton(mouse)) {
                    settingButtonRect.color = ColorConstant.releaseColor
                } else {
                    rootItemBackground.color = ColorConstant.releaseColor
                    if(comboBoxPopup.opened) {
                        accountComboBox.popup.close()
                    } else {
                        accountComboBox.popup.open()
                    }
                }
            }
            onEntered: { rootItemBackground.color = ColorConstant.hoverColor; }
            onExited: { rootItemBackground.color = "white";}
            onMouseXChanged: {
                // manually make settings button hover
                if(isMouseOnSettingsButton(mouse)){
                    settingButtonRect.color = ColorConstant.hoverColor;
                    rootItemBackground.color = "white";
                } else {
                    settingButtonRect.color = "white";
                    rootItemBackground.color = ColorConstant.hoverColor;
                }
            }
            function isMouseOnSettingsButton(mouse){
                var mousePos = mapToItem(comboBoxRootMouseArea, mouse.x, mouse.y)
                var settingsButtonPos = mapToItem(comboBoxRootMouseArea, settingsButton.x, settingsButton.y)
                if((mousePos.x >= settingsButtonPos.x && mousePos.x <= settingsButtonPos.x + settingsButton.width) &&
                   (mousePos.y >= settingsButtonPos.y && mousePos.y <= settingsButtonPos.y + settingsButton.height))
                    return true
                return false
            }
        }

        indicator: null

        // overwrite the combo box pop up to add footer (for add accounts)
        popup: Popup {
            id: comboBoxPopup

            y: accountComboBox.height - 1
            implicitWidth: accountComboBox.width - 1
            // hack - limite the accounts that can be shown
            implicitHeight: Math.min(accountComboBox.height * Math.min(5, accountListModel.rowCount()), sidePanelRect.height)
            padding: 0

            contentItem: ListView {
                id: comboBoxPopupListView

                clip: true
                model: accountListModel
                implicitHeight: contentHeight
                delegate:ItemDelegate {

                    Image {
                        id: userImage

                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        width: 30
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        // in list view, index a signal interger
                        source: "image://accountImage/account_" + index + "_0_popup"
                    }

                    Text {
                        id: textUserAliasPopup

                        anchors.left: userImage.right
                        anchors.leftMargin: 10
                        anchors.top: itemCoboBackground.top
                        anchors.topMargin: 5

                        text: textMetricsUserAliasPopup.elidedText
                        font.pointSize: textFontSize
                    }

                    Text {
                        id: textUsernamePopup

                        anchors.left: userImage.right
                        anchors.leftMargin: 10
                        anchors.top: textUserAliasPopup.bottom
                        anchors.topMargin: 5

                        text: textMetricsUsernamePopup.elidedText
                        font.pointSize: textFontSize
                        color: ColorConstant.faddedFontColor
                    }

                    TextMetrics {
                        id: textMetricsUserAliasPopup
                        elide: Text.ElideMiddle
                        elideWidth: accountComboBox.width - userImage.width - settingsButton.width - 30
                        text: Alias
                    }

                    TextMetrics {
                        id: textMetricsUsernamePopup
                        elide: Text.ElideMiddle
                        elideWidth: accountComboBox.width - userImage.width - settingsButton.width - 30
                        text: Username
                    }

                    background: Rectangle {
                        id: itemCoboBackground

                        implicitWidth: accountComboBox.width
                        implicitHeight: accountComboBox.height
                        border.width: 0
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: { itemCoboBackground.color = ColorConstant.pressColor; }
                        onReleased: { itemCoboBackground.color = ColorConstant.releaseColor; }
                        onEntered: { itemCoboBackground.color = ColorConstant.hoverColor; }
                        onExited: { itemCoboBackground.color = "white"; }
                    }
                }

                footer: HoverableButton {
                    id: comboBoxFooterItem

                    implicitWidth: accountComboBox.width
                    implicitHeight: accountComboBox.height

                    text: qsTr("Add Account") + "+"
                    backgroundColor: "white"
                    onExitColor: "white"
                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
            background: Rectangle {
                id: accountComboBoxPopup

                CustomBorder {
                    commonBorder: false
                    lBorderwidth: 0
                    rBorderwidth: 1
                    tBorderwidth: 0
                    bBorderwidth: 1
                    borderColor: ColorConstant.tabbarBorderColor
                }
            }
        }
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
                color: pageOne.down ? "white" : ColorConstant.releaseColor
                border.color: ColorConstant.tabbarBorderColor

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
                    onPressed: { buttonRectOne.color = ColorConstant.pressColor; tabBar.currentIndex = 0; pageOne.down = true; pageTwo.down = false;}
                    onReleased: { buttonRectOne.color = ColorConstant.releaseColor; }
                    onEntered: { buttonRectOne.color = ColorConstant.hoverColor; }
                    onExited: { buttonRectOne.color = Qt.binding(function() { return pageOne.down ? "white" : ColorConstant.releaseColor }); }
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
                color: pageTwo.down ? "white" : ColorConstant.releaseColor
                border.color: ColorConstant.tabbarBorderColor

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
                    onPressed: { buttonRectTwo.color = ColorConstant.pressColor; tabBar.currentIndex = 1; pageTwo.down = true; pageOne.down = false;}
                    onReleased: { buttonRectTwo.color = ColorConstant.releaseColor; }
                    onEntered: { buttonRectTwo.color = ColorConstant.hoverColor; }
                    onExited: { buttonRectTwo.color = Qt.binding(function() { return pageTwo.down ? "white" : ColorConstant.releaseColor }); }
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
        border.color: ColorConstant.tabbarBorderColor
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
                    border.color: ColorConstant.pressColor
                    radius: 10
                    color: contactSearchBar.activeFocus ? "white" : ColorConstant.hoverColor

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

                            color: contactSearchBar.activeFocus ? "white" : ColorConstant.hoverColor
                       }
                        onTextChanged: {
                            mainViewWindow.searchBarTextChanged(contactSearchBar.text)
                        }
                    }
                }

                ListView {
                    id: smartListView

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    width: parent.width
                    height: parent.height -  contactSearchBarRect.height - 15
                    model: animalModel
                    clip: true

                    delegate: ItemDelegate {

                        Image {
                            id: smartListUserImage

                            anchors.left: parent.left
                            anchors.verticalCenter: parent.verticalCenter
                            width: 30
                            height: parent.height
                            fillMode: Image.PreserveAspectFit
                            source: "images/jami.png"
                        }

                        Text {
                            id: smartListUserName

                            anchors.left: smartListUserImage.right
                            anchors.leftMargin: 10
                            anchors.top: parent.top
                            fontSizeMode: Text.Fit

                            text: type
                            font.pointSize: 10
                        }

                        Text {
                            id: smartListUserSize

                            anchors.bottom: parent.bottom
                            anchors.left: smartListUserImage.right
                            anchors.leftMargin: 10
                            fontSizeMode: Text.Fit

                            text: size
                            font.pointSize: 10
                        }

                        background: Rectangle {
                            id: itemSmartListBackground

                            implicitWidth: smartListView.width
                            implicitHeight: smartListUserName.height + smartListUserSize.height + 10
                            border.width: 0
                        }

                        MouseArea {
                            anchors.fill: parent;
                            hoverEnabled: true;
                            acceptedButtons: Qt.LeftButton | Qt.RightButton

                            onPressed: { itemSmartListBackground.color = ColorConstant.pressColor; }
                            onReleased: {
                                itemSmartListBackground.color = ColorConstant.releaseColor
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
                            onEntered: { itemSmartListBackground.color = ColorConstant.hoverColor; }
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
                                    color: smartListCopyItem.down ? ColorConstant.releaseColor : "white"
                                    MouseArea {
                                        anchors.fill: parent;
                                        hoverEnabled: true;
                                        onPressed: { smartListContextMenuBackRect.color = ColorConstant.pressColor; }
                                        onReleased: {
                                            smartListContextMenuBackRect.color = ColorConstant.releaseColor
                                            smartListContextMenu.close()
                                            contextMenuPopUpMock.open()
                                        }
                                        onEntered: { smartListContextMenuBackRect.color = ColorConstant.hoverColor; }
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

                        parent: smartListView
                        anchors.top: smartListView.top
                        anchors.right: smartListView.right
                        size: 0.5
                        orientation: Qt.Vertical
                        policy: smartListView.contentHeight > smartListView.height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

                        contentItem: Rectangle {
                            implicitWidth: 5
                            implicitHeight: 100
                            radius: width / 2
                            color: controlBar.pressed ? "#81e889" : "#c2f4c6"
                        }
                        background: Rectangle {
                            color: ColorConstant.transparentColor
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
