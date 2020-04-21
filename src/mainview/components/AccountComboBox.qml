import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

ComboBox {
    id: accountComboBox

    signal accountChanged(int index)
    signal needToBackToWelcomePage(int index)
    signal needToUpdateSmartList(string accountId)

    currentIndex: 0

    function backToWelcomePage(index) {
        accountComboBox.needToBackToWelcomePage(index)
    }

    function updateSmartList(accountId) {
        accountComboBox.needToUpdateSmartList(accountId)
    }

    function updateAccountListModel() {
        accountListModel.dataChanged(accountListModel.index(0, 0),
                                     accountListModel.index(
                                         accountListModel.rowCount() - 2, 0))
    }

    Image {
        id: userImageRoot

        anchors.left: accountComboBox.left
        anchors.leftMargin: 5
        anchors.verticalCenter: accountComboBox.verticalCenter

        width: accountComboBox.height - 10
        height: accountComboBox.height - 10

        fillMode: Image.PreserveAspectFit
        source: "data:image/png;base64," + accountListModel.data(
                    accountListModel.index(accountComboBox.currentIndex, 0),
                    259)
        mipmap: true

        Rectangle {
            id: presenseRect

            anchors.right: userImageRoot.right
            anchors.rightMargin: 1
            anchors.bottom: userImageRoot.bottom
            anchors.bottomMargin: 2

            width: 14
            height: 14

            // enum REGISTERED == 5
            visible: accountListModel.data(accountListModel.index(
                                               accountComboBox.currentIndex,
                                               0), 261) === 5

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
    }

    Text {
        id: textUserAliasRoot

        anchors.left: userImageRoot.right
        anchors.leftMargin: 10
        anchors.top: rootItemBackground.top
        anchors.topMargin: 5

        text: textMetricsUserAliasRoot.elidedText
        font.pointSize: JamiTheme.textFontSize
    }

    Text {
        id: textUsernameRoot

        anchors.left: userImageRoot.right
        anchors.leftMargin: 10
        anchors.top: textUserAliasRoot.bottom
        anchors.topMargin: 5

        text: textMetricsUsernameRoot.elidedText
        font.pointSize: JamiTheme.textFontSize
        color: JamiTheme.faddedFontColor
    }

    TextMetrics {
        id: textMetricsUserAliasRoot

        font: textUserAliasRoot.font
        elide: Text.ElideMiddle
        elideWidth: accountComboBox.width - userImageRoot.width - settingsButton.width - 25
        text: accountListModel.data(accountListModel.index(
                                        accountComboBox.currentIndex, 0), 257)
    }

    TextMetrics {
        id: textMetricsUsernameRoot

        font: textUsernameRoot.font
        elide: Text.ElideMiddle
        elideWidth: accountComboBox.width - userImageRoot.width - settingsButton.width - 25
        text: accountListModel.data(accountListModel.index(
                                        accountComboBox.currentIndex, 0), 258)
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
            if (isMouseOnSettingsButton(mouse))
                settingButtonRect.color = JamiTheme.pressColor
            else
                rootItemBackground.color = JamiTheme.pressColor
        }
        onReleased: {
            if (isMouseOnSettingsButton(mouse)) {
                settingButtonRect.color = JamiTheme.releaseColor
            } else {
                rootItemBackground.color = JamiTheme.releaseColor
                if (comboBoxPopup.opened) {
                    accountComboBox.popup.close()
                } else {
                    accountComboBox.popup.open()
                }
            }
        }
        onEntered: {
            rootItemBackground.color = JamiTheme.hoverColor
        }
        onExited: {
            rootItemBackground.color = "white"
        }
        onMouseXChanged: {
            // manually make settings button hover
            if (isMouseOnSettingsButton(mouse)) {
                settingButtonRect.color = JamiTheme.hoverColor
                rootItemBackground.color = "white"
            } else {
                settingButtonRect.color = "white"
                rootItemBackground.color = JamiTheme.hoverColor
            }
        }
        function isMouseOnSettingsButton(mouse) {
            var mousePos = mapToItem(comboBoxRootMouseArea, mouse.x, mouse.y)
            var settingsButtonPos = mapToItem(comboBoxRootMouseArea,
                                              settingsButton.x,
                                              settingsButton.y)
            if ((mousePos.x >= settingsButtonPos.x
                 && mousePos.x <= settingsButtonPos.x + settingsButton.width)
                    && (mousePos.y >= settingsButtonPos.y
                        && mousePos.y <= settingsButtonPos.y + settingsButton.height))
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
        implicitHeight: Math.min(accountComboBox.height * Math.min(
                                     5, accountListModel.rowCount()),
                                 sidePanelRect.height)
        padding: 0

        contentItem: ListView {
            id: comboBoxPopupListView

            clip: true
            model: accountListModel
            implicitHeight: contentHeight
            delegate: ItemDelegate {

                Image {
                    id: userImage

                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter

                    width: 30
                    height: parent.height

                    fillMode: Image.PreserveAspectFit
                    mipmap: true
                    // in list view, index a signal interger
                    source: "data:image/png;base64," + accountListModel.data(
                                accountListModel.index(index, 0), 259)
                }

                Text {
                    id: textUserAliasPopup

                    anchors.left: userImage.right
                    anchors.leftMargin: 10
                    anchors.top: itemCoboBackground.top
                    anchors.topMargin: 5

                    text: textMetricsUserAliasPopup.elidedText
                    font.pointSize: JamiTheme.textFontSize
                }

                Text {
                    id: textUsernamePopup

                    anchors.left: userImage.right
                    anchors.leftMargin: 10
                    anchors.top: textUserAliasPopup.bottom
                    anchors.topMargin: 5

                    text: textMetricsUsernamePopup.elidedText
                    font.pointSize: JamiTheme.textFontSize
                    color: JamiTheme.faddedFontColor
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
                    onPressed: {
                        itemCoboBackground.color = JamiTheme.pressColor
                    }
                    onReleased: {
                        itemCoboBackground.color = JamiTheme.releaseColor
                        currentIndex = index
                        comboBoxPopup.close()
                        accountComboBox.accountChanged(index)
                    }
                    onEntered: {
                        itemCoboBackground.color = JamiTheme.hoverColor
                    }
                    onExited: {
                        itemCoboBackground.color = "white"
                    }
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

            ScrollIndicator.vertical: ScrollIndicator {}
        }
        background: Rectangle {
            id: accountComboBoxPopup

            CustomBorder {
                commonBorder: false
                lBorderwidth: 0
                rBorderwidth: 1
                tBorderwidth: 0
                bBorderwidth: 1
                borderColor: JamiTheme.tabbarBorderColor
            }
        }
    }
}
