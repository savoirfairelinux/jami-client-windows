import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import Qt.model.account 1.0
import Qt.constant.color 1.0

Window {
    id: mainViewWindow

    property int minWidth: 320
    property int minHeight: 400
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 580
    minimumWidth: minWidth
    minimumHeight: minHeight

    AccountListModel {
        id: accountListModel
    }

    Component {
        id: popback
        Rectangle {
            Button {
                id: backButton

                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                text: "Back"
                onClicked: {
                    var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                        return index > 0
                    })
                    var inCallViewStack = callViewStack.find(function(item, index) {
                        return index > 0
                    })
                    if(inWelcomeViewStack)
                        welcomeViewStack.pop()
                    else
                        callViewStack.pop()
                }
            }

            Text {
                anchors.left: backButton.right
                anchors.leftMargin: 10
                anchors.verticalCenter: backButton.verticalCenter
                fontSizeMode: Text.Fit

                font.pointSize: 10
                text: userTypeName
            }
        }
    }

    Component {
        id: mainViewWindowRectComp

        Rectangle {
            id: sidePanelRect

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
                }

                TextMetrics {
                    id: textMetricsUserAliasRoot
                    elide: Text.ElideRight
                    elideWidth: accountComboBox.width - userImageRoot.width - 10
                    text: accountListModel.data(accountListModel.index(0,0), 257)
                }

                TextMetrics {
                    id: textMetricsUsernameRoot
                    elide: Text.ElideRight
                    elideWidth: accountComboBox.width - userImageRoot.width - 10
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
                            if(accountComboBox.down) {
                                accountComboBox.popup.close()
                            } else {
                                accountComboBox.popup.open()
                            }
                        }
                    }
                    onEntered: { rootItemBackground.color = ColorConstant.hoverColor; }
                    onExited: { rootItemBackground.color = ColorConstant.lightGreyWhite;}
                    onMouseXChanged: {
                        // manually make settings button hover
                        if(isMouseOnSettingsButton(mouse)){
                            settingButtonRect.color = ColorConstant.hoverColor;
                            rootItemBackground.color = ColorConstant.lightGreyWhite;
                        } else {
                            settingButtonRect.color = ColorConstant.lightGreyWhite;
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
                            }

                            TextMetrics {
                                id: textMetricsUserAliasPopup
                                elide: Text.ElideRight
                                elideWidth: accountComboBox.width - userImage.width - 10
                                text: Alias
                            }

                            TextMetrics {
                                id: textMetricsUsernamePopup
                                elide: Text.ElideRight
                                elideWidth: accountComboBox.width - userImage.width - 10
                                text: Username
                            }

                            background: Rectangle {
                                id: itemCoboBackground

                                implicitWidth: accountComboBox.width
                                implicitHeight: accountComboBox.height
                                border.width: 0

                                CustomBorder {
                                    commonBorder: false
                                    lBorderwidth: 0
                                    rBorderwidth: 0
                                    tBorderwidth: 1
                                    bBorderwidth: 1
                                    borderColor: ColorConstant.hoverColor
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: { itemCoboBackground.color = ColorConstant.pressColor; }
                                onReleased: { itemCoboBackground.color = ColorConstant.releaseColor; }
                                onEntered: { itemCoboBackground.color = ColorConstant.hoverColor; }
                                onExited: { itemCoboBackground.color = ColorConstant.lightGreyWhite; }
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
                anchors.topMargin: 20
                anchors.left: sidePanelRect.left
                anchors.leftMargin: 5

                width: sidePanelRect.width
                height: Math.max(pageOne.height, pageTwo.height)
                currentIndex: 0

                TabButton {
                    id: pageOne

                    width: tabBar.width / 2 - 5
                    height: textConvElement.height + 10
                    down: true

                    background: Rectangle {
                        id: buttonRectOne

                        radius: 10
                        color: pageOne.down ? ColorConstant.releaseColor : ColorConstant.lightGreyWhite

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
                            onPressed: { buttonRectOne.color = ColorConstant.pressColor; tabBar.currentIndex = 0; pageOne.down = true; pageTwo.down = false;}
                            onReleased: { buttonRectOne.color = ColorConstant.releaseColor; }
                            onEntered: { buttonRectOne.color = ColorConstant.hoverColor; }
                            onExited: { buttonRectOne.color = Qt.binding(function() { return pageOne.down ? ColorConstant.releaseColor : ColorConstant.lightGreyWhite }); }
                        }
                    }
                }
                TabButton {
                    id: pageTwo

                    width: tabBar.width / 2 - 5
                    height: textInvElement.height + 10

                    background: Rectangle {
                        id: buttonRectTwo

                        radius: 10
                        color: pageTwo.down ? ColorConstant.releaseColor : ColorConstant.lightGreyWhite

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
                            onPressed: { buttonRectTwo.color = ColorConstant.pressColor; tabBar.currentIndex = 1; pageTwo.down = true; pageOne.down = false;}
                            onReleased: { buttonRectTwo.color = ColorConstant.releaseColor; }
                            onEntered: { buttonRectTwo.color = ColorConstant.hoverColor; }
                            onExited: { buttonRectTwo.color = Qt.binding(function() { return pageTwo.down ? ColorConstant.releaseColor : ColorConstant.lightGreyWhite }); }
                        }
                    }
                }
            }

            StackLayout {
                id: stackLayoutView

                anchors.top: tabBar.bottom
                anchors.topMargin: 10
                currentIndex: tabBar.currentIndex
                width: sidePanelRect.width
                height: sidePanelRect.height - accountComboBox.height - tabBar.height

                Item {
                    id: stackOne

                    anchors.fill: parent

                    // search bar container to embed search label
                    Rectangle {
                        id: contactSearchBarRect

                        anchors.top: parent.top
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
                                onExited: { itemSmartListBackground.color = ColorConstant.lightGreyWhite; }
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
                                        color: smartListCopyItem.down ? ColorConstant.releaseColor : ColorConstant.lightGreyWhite
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
                                            onExited: { smartListContextMenuBackRect.color = ColorConstant.lightGreyWhite; }
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
    }
    Component {
        id: welcomeRectComp

        Rectangle {
            id: welcomeRect

            Label {
                id: welcomeLabel

                anchors.centerIn: welcomeRect
                width: 200
                height: 200
                text: "Welcome"
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 22
                font.italic: true
            }
            Button {
                id: changeLabelTextButton

                width: 100
                height: 50
                anchors.top: welcomeLabel.bottom
                anchors.topMargin: 10
                anchors.left: welcomeLabel.left
                anchors.leftMargin: welcomeLabel.width / 2 - changeLabelTextButton.width / 2

                text: changeLabelTextButtonText
                font.pointSize: 8
                onClicked: {
                    mainViewWindow.changeLabelTextButtonClicked(changeLabelTextButtonText);
                    // create a new window (qml ui)
                    var component = Qt.createComponent("StartLoading.qml")
                    var window    = component.createObject(mainViewWindow)
                    window.show()
                }
            }
        }
    }

    Rectangle {
        id: splitView

        anchors.fill: parent
        width: mainViewWindow.width
        height: mainViewWindow.height

        StackView {
            id: callViewStack

            initialItem: mainViewWindowRectComp
            anchors.left: splitView.left
            width: splitView.width / 2
            height: splitView.height

            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
        }

        StackView {
            id: welcomeViewStack

            initialItem: welcomeRectComp
            anchors.right: parent.right
            width: splitView.width / 2
            height: splitView.height

            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
        }
    }

    onWidthChanged: {
        if(mainViewWindow.width < 500){
            welcomeViewStack.visible = false
            callViewStack.width = mainViewWindow.width

            var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                return index > 0
            })
            if(inWelcomeViewStack) {
                callViewStack.push(popback)
                welcomeViewStack.pop()
            }
        } else if(mainViewWindow.width >= 500) {
            var inCallViewStack = callViewStack.find(function(item, index) {
                return index > 0
            })
            if(inCallViewStack) {
                welcomeViewStack.push(popback)
                callViewStack.pop()
            }

            welcomeViewStack.visible = true
            callViewStack.width = mainViewWindow.width / 2
        }
    }

    Dialog {
        id: contextMenuPopUpMock

        x: Math.round((mainViewWindow.width - width) / 2)
        y: Math.round((mainViewWindow.height - height) / 2)
        width: Math.min(mainViewWindow.width, mainViewWindow.height) / 3 * 2
        // when dialog is opened, trigger mainViewWindow overlay which is defined in overlay.model (model : true is necessary)
        modal: true

        standardButtons: Dialog.Ok | Dialog.Cancel

        contentHeight: popImg.height

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
