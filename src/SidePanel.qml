import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Universal 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: callViewRect

    property int minWidth: 380
    property int minHeight: 500
    property int textFontSize: 8

    implicitWidth: minWidth
    implicitHeight: minHeight

    ComboBox {
        id: accountComboBox

        anchors.top: callViewRect.top
        width: callViewRect.width
        height: 80
        currentIndex: 0

        Image {
            id: userImageRoot

            anchors.left: accountComboBox.left
            anchors.verticalCenter: accountComboBox.verticalCenter
            width: 50
            height: accountComboBox.height
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            source: "image://accountImage/account_0_0"
        }

        Text {
            id: textElementRoot

            anchors.left: userImageRoot.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: accountListModel.data(accountListModel.index(0,0), 257)
            font.pointSize: 8
            antialiasing : false
        }

        Button {
            id: settingsButton

            anchors.right: accountComboBox.right
            anchors.rightMargin: 10
            anchors.verticalCenter: accountComboBox.verticalCenter
            width: 30
            height: 30

            background: Rectangle {
                id: settingButtonRect

                radius: 30

                Image {
                    id: settingsButtonImage

                    anchors.centerIn: settingButtonRect
                    width: settingsButton.width - 2
                    height: settingsButton.height - 2
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/images/icons/round-settings-24px.svg"
                }
            }
        }

        background: Rectangle {
            id: rootItemBackground

            implicitWidth: accountComboBox.width
            implicitHeight: accountComboBox.height
            border.color: accountComboBox.pressed ? "#17a81a" : "#21be2b"
            border.width: accountComboBox.visualFocus ? 2 : 1
            radius: 2
        }

        MouseArea {
            id: comboBoxRootMouseArea
            anchors.fill: parent;
            hoverEnabled: true;
            propagateComposedEvents: true
            onPressed: {
                if(isMouseOnSettingsButton(mouse))
                    settingButtonRect.color = "#c0c0c0"
                else
                    rootItemBackground.color = "#c0c0c0"
            }
            onReleased: {
                if(isMouseOnSettingsButton(mouse)) {
                    settingButtonRect.color = "#e0e0e0"
                } else {
                    rootItemBackground.color = "#e0e0e0"
                    if(accountComboBox.down) {
                        accountComboBox.popup.close()
                    } else {
                        accountComboBox.popup.open()
                    }
                }
            }
            onEntered: { rootItemBackground.color = "#c7c7c7"; }
            onExited: { rootItemBackground.color = Qt.binding(function() { return rootItemBackground.down ? "#e0e0e0" :"#fdfdfd" }); }
            onMouseXChanged: {
                // manually make settings button hover
                if(isMouseOnSettingsButton(mouse)){
                    settingButtonRect.color = "#c7c7c7";
                    rootItemBackground.color = Qt.binding(function() { return rootItemBackground.down ? "#e0e0e0" :"#fdfdfd" });
                } else {
                    settingButtonRect.color = Qt.binding(function() { return settingButtonRect.down ? "#e0e0e0" :"#fdfdfd" });
                    rootItemBackground.color = "#c7c7c7";
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

            y: accountComboBox.height
            width: accountComboBox.width
            implicitHeight: contentItem.implicitHeight
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
                        width: 50
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                        asynchronous: true
                        // in list view, index a signal interger
                        source: "image://accountImage/account_" + index + "_0_popup"
                    }

                    Text {
                        anchors.left: userImage.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: alias
                        font.pointSize: 10
                    }

                    background: Rectangle {
                        id: itemCoboBackground

                        implicitWidth: accountComboBox.width
                        implicitHeight: accountComboBox.height
                        border.width: 0
                    }

                    MouseArea {
                        anchors.fill: parent;
                        hoverEnabled: true;
                        onPressed: { itemCoboBackground.color = "#c0c0c0"; }
                        onReleased: {
                            itemCoboBackground.color = "#e0e0e0"
                        }
                        onEntered: { itemCoboBackground.color = "#c7c7c7"; }
                        onExited: { itemCoboBackground.color = Qt.binding(function() { return itemCoboBackground.down ? "#e0e0e0" :"#fdfdfd" }); }
                    }
                }

                footer: Button {
                    id: comboBoxFooterItem

                    implicitWidth: accountComboBox.width
                    implicitHeight: accountComboBox.height

                    Text {
                        id: footerTextContent

                        anchors.centerIn: comboBoxFooterItem
                        text: qsTr("Add Account")
                        font.pointSize: 10
                        fontSizeMode: Text.Fit
                    }
                }

                ScrollIndicator.vertical: ScrollIndicator { }
            }
        }
    }

    TabBar {
        id: tabBar

        anchors.top: accountComboBox.bottom
        anchors.topMargin: 30
        width: callViewRect.width
        height: Math.max(pageOne.height, pageTwo.height)
        currentIndex: 0

        TabButton {
            id: pageOne

            width: tabBar.width / 2
            height: textConvElement.height + 10
            down: true

            background: Rectangle {
                id: buttonRectOne

                radius: 10
                color: pageOne.down ? "#e0e0e0" :"#fdfdfd"

                Text {
                    id: textConvElement

                    anchors.centerIn: buttonRectOne

                    text: qsTr("Converstation")
                    font.pointSize: 10
                    fontSizeMode: Text.Fit
                    opacity: enabled ? 1.0 : 0.3
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { buttonRectOne.color = "#c0c0c0"; tabBar.currentIndex = 0; pageOne.down = true; pageTwo.down = false;}
                    onReleased: { buttonRectOne.color = "#e0e0e0"; }
                    onEntered: { buttonRectOne.color = "#c7c7c7"; }
                    onExited: { buttonRectOne.color = Qt.binding(function() { return pageOne.down ? "#e0e0e0" :"#fdfdfd" }); }
                }
            }
        }
        TabButton {
            id: pageTwo

            width: tabBar.width / 2
            height: textInvElement.height + 10

            background: Rectangle {
                id: buttonRectTwo

                radius: 10
                color: pageTwo.down ? "#e0e0e0" :"#fdfdfd"

                Text {
                    id: textInvElement

                    anchors.centerIn: buttonRectTwo

                    text: qsTr("Invitation")
                    font.pointSize: 10
                    opacity: enabled ? 1.0 : 0.3
                    fontSizeMode: Text.Fit
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { buttonRectTwo.color = "#c0c0c0"; tabBar.currentIndex = 1; pageTwo.down = true; pageOne.down = false;}
                    onReleased: { buttonRectTwo.color = "#e0e0e0"; }
                    onEntered: { buttonRectTwo.color = "#c7c7c7"; }
                    onExited: { buttonRectTwo.color = Qt.binding(function() { return pageTwo.down ? "#e0e0e0" :"#fdfdfd" }); }
                }
            }
        }
    }

    StackLayout {
        id: stackLayoutView

        anchors.top: tabBar.bottom
        anchors.topMargin: 10
        currentIndex: tabBar.currentIndex
        width: callViewRect.width
        height: callViewRect.height - accountComboBox.height - tabBar.height

        Item {
            id: stackOne

            anchors.fill: parent

            TextField {
                id: searchBar

                anchors.top: parent.top
                width: parent.width
                height: 30
                font.pointSize: 10

                placeholderText: qsTr("Search")
                background: Rectangle {
                    id: searchBarBackground

                    border.color: searchBar.focus ? "green" : "black"
                }
                onTextChanged: {
                    mainWindow.searchBarTextChanged(searchBar.text)
                }
            }

            ListView {
                id: smartListView

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width
                height: parent.height -  searchBar.height - 15
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
                        asynchronous: true
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

                        onPressed: { itemSmartListBackground.color = "#c0c0c0"; }
                        onReleased: {
                            itemSmartListBackground.color = "#e0e0e0"
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
                        onEntered: { itemSmartListBackground.color = "#c7c7c7"; }
                        onExited: { itemSmartListBackground.color = Qt.binding(function() { return itemSmartListBackground.down ? "#e0e0e0" :"#fdfdfd" }); }
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
                                color: smartListCopyItem.down ? "#e0e0e0" :"#fdfdfd"
                                MouseArea {
                                    anchors.fill: parent;
                                    hoverEnabled: true;
                                    onPressed: { smartListContextMenuBackRect.color = "#c0c0c0"; }
                                    onReleased: {
                                        smartListContextMenuBackRect.color = "#e0e0e0"
                                        smartListContextMenu.close()
                                        contextMenuPopUpMock.open()
                                    }
                                    onEntered: { smartListContextMenuBackRect.color = "#c7c7c7"; }
                                    onExited: { smartListContextMenuBackRect.color = Qt.binding(function() { return smartListCopyItem.down ? "#e0e0e0" :"#fdfdfd" }); }
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
                        color: "transparent"
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
