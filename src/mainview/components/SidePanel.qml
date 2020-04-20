import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0
import net.jami.AccountComboBoxQmlObjectHolder 1.0
import net.jami.ConversationSmartListViewQmlObjectHolder 1.0

import "../../commoncomponents"

Rectangle {
    id: sidePanelRect

    signal conversationSmartListNeedToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias, string currentUID, bool inCall, bool isIncomingCallInProgress)
    signal accountComboBoxNeedToShowWelcomePage(int index)
    signal conversationSmartListViewNeedToShowWelcomePage

    function deselectConversationSmartList() {
        conversationSmartListView.currentIndex = -1
    }

    // intended -> since strange behavior will happen without this for stackview
    anchors.fill: parent

    AccountComboBoxQmlObjectHolder {
        id: accountComboBoxQmlObjectHolder

        onAccountSignalsReconnect: {
            CallCenter.connectCallstatusChangedSignal(accountId)
            conversationSmartListViewQmlObjectHolder.connectConversationModel()
        }
    }

    ConversationSmartListViewQmlObjectHolder {
        id: conversationSmartListViewQmlObjectHolder
    }

    AccountComboBox {
        id: accountComboBox

        anchors.top: sidePanelRect.top

        width: sidePanelRect.width
        height: 60

        currentIndex: 0

        onAccountChanged: {
            accountComboBoxQmlObjectHolder.accountChanged(index)
            contactSearchBar.clear()
        }

        onNeedToUpdateSmartList: {
            conversationSmartListView.currentIndex = -1
            conversationSmartListView.updateSmartList(accountId)
        }

        onNeedToBackToWelcomePage: {
            sidePanelRect.accountComboBoxNeedToShowWelcomePage(index)
        }

        Component.onCompleted: {
            accountComboBoxQmlObjectHolder.setAccountComboBoxQmlObject(
                        accountComboBox)
            accountComboBoxQmlObjectHolder.accountChanged(0)
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
                color: pageOne.down ? "white" : JamiTheme.releaseColor
                border.color: JamiTheme.tabbarBorderColor

                Text {
                    id: textConvElement

                    anchors.centerIn: buttonRectOne

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter

                    text: qsTr("Converstation")
                    font.pointSize: JamiTheme.textFontSize
                    opacity: enabled ? 1.0 : 0.3
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        buttonRectOne.color = JamiTheme.pressColor
                        tabBar.currentIndex = 0
                        pageOne.down = true
                        pageTwo.down = false
                    }
                    onReleased: {
                        buttonRectOne.color = JamiTheme.releaseColor
                    }
                    onEntered: {
                        buttonRectOne.color = JamiTheme.hoverColor
                    }
                    onExited: {
                        buttonRectOne.color = Qt.binding(function () {
                            return pageOne.down ? "white" : JamiTheme.releaseColor
                        })
                    }
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

                    font.pointSize: JamiTheme.textFontSize

                    text: qsTr("Invitation")
                    opacity: enabled ? 1.0 : 0.3
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        buttonRectTwo.color = JamiTheme.pressColor
                        tabBar.currentIndex = 1
                        pageTwo.down = true
                        pageOne.down = false
                    }
                    onReleased: {
                        buttonRectTwo.color = JamiTheme.releaseColor
                    }
                    onEntered: {
                        buttonRectTwo.color = JamiTheme.hoverColor
                    }
                    onExited: {
                        buttonRectTwo.color = Qt.binding(function () {
                            return pageTwo.down ? "white" : JamiTheme.releaseColor
                        })
                    }
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

            width: pageOne.width + pageTwo.width - 9
            height: 1

            color: "white"
        }

        StackLayout {
            id: stackLayoutView

            anchors.centerIn: stackLayoutRect

            width: stackLayoutRect.width - 10
            height: stackLayoutRect.height - 20

            currentIndex: tabBar.currentIndex

            Item {
                id: stackOne

                Layout.fillHeight: stackLayoutView
                Layout.fillWidth: stackLayoutView

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
                            visible: !contactSearchBar.text
                                     && !contactSearchBar.activeFocus
                        }

                        background: Rectangle {
                            id: searchBarBackground

                            color: contactSearchBar.activeFocus ? "white" : JamiTheme.hoverColor
                        }
                        onTextChanged: {
                            mainViewWindow.searchBarTextChanged(
                                        contactSearchBar.text)
                        }
                    }
                }

                ConversationSmartListView {
                    id: conversationSmartListView

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left

                    width: parent.width
                    height: parent.height - contactSearchBarRect.height - 15

                    onNeedToBackToWelcomePage: {
                        sidePanelRect.conversationSmartListViewNeedToShowWelcomePage()
                    }

                    onNeedToAccessMessageWebView: {
                        sidePanelRect.conversationSmartListNeedToAccessMessageWebView(
                                    currentUserDisplayName, currentUserAlias,
                                    currentUID, inCall,
                                    isIncomingCallInProgress)
                    }

                    Component.onCompleted: {
                        conversationSmartListViewQmlObjectHolder.setConversationSmartListViewQmlObjectHolder(
                                    conversationSmartListView)
                        conversationSmartListView.currentIndex = -1
                    }
                }
            }

            Item {
                id: stackTwo

                Layout.fillHeight: stackLayoutView
                Layout.fillWidth: stackLayoutView

                Image {
                    id: popImg

                    anchors.centerIn: parent
                    width: 50
                    height: 50
                    fillMode: Image.PreserveAspectFit
                    source: "qrc:/images/jami.png"
                }
            }
        }
    }
}
