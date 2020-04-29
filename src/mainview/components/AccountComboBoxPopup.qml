import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0

import "../../commoncomponents"

Popup {
    id: comboBoxPopup

    signal accountNeedToChange(int index)

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
                    comboBoxPopup.accountNeedToChange(index)
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
