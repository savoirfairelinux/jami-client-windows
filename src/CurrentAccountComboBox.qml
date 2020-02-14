import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls.Universal 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: currentAccountComboBoxRect

    property int minWidth: 300
    property int minHeight: 60
    property int textFontSize: 8
    property string accountImageSource: "Text"

    implicitWidth: minWidth
    implicitHeight: minHeight

    ComboBox {
        id: accountComboBox

        anchors.top: currentAccountComboBoxRect.top
        width: currentAccountComboBoxRect.width
        height: currentAccountComboBoxRect.height
        currentIndex: 0

        Image {
            id: userImageRoot

            anchors.left: accountComboBox.left
            anchors.verticalCenter: accountComboBox.verticalCenter
            width: 30
            height: accountComboBox.height
            fillMode: Image.PreserveAspectFit
            source: "image://accountImage/account_0_0"
        }

        Text {
            id: textElementRoot

            anchors.left: userImageRoot.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            text: accountListModel.data(accountListModel.index(0,0), 257)
            font.pointSize: 10
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
                        console.log("accountComboBox.popup.open()")
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
                        width: 30
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                        source: accountImageSource
                    }

                    Text {
                        anchors.left: userImage.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: alias + " " + username
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
}
