import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0
import net.jami.AccountAdapter 1.0
import net.jami.ConversationSmartListViewQmlObjectHolder 1.0
import net.jami.ContactSearchBarQmlObjectHolder 1.0

import "../../commoncomponents"

Rectangle {
    id: sidePanelRect

    property bool tabBarVisible: true
    property int pendingRequestCount: 0
    property int totalUnreadMessagesCount: 0

    signal conversationSmartListNeedToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias, string currentUID, bool callStackViewShouldShow)
    signal accountComboBoxNeedToShowWelcomePage(int index)
    signal conversationSmartListViewNeedToShowWelcomePage
    signal accountSignalsReconnect(string accountId)
    signal needToUpdateConversationForAddedContact

    // Hack -> force redraw
    function forceReselectConversationSmartListCurrentIndex() {
        var index = conversationSmartListView.currentIndex
        conversationSmartListView.currentIndex = -1
        conversationSmartListView.currentIndex = index
    }

    // for contact request conv to be focused correctly
    function setCurrentUidSmartListModelIndex() {
        conversationSmartListView.currentIndex
                = conversationSmartListView.model.currentUidSmartListModelIndex(
                    )
    }

    function updatePendingRequestCount() {
        pendingRequestCount = utilsAdapter.getTotalPendingRequest()
    }

    function updateTotalUnreadMessagesCount() {
        totalUnreadMessagesCount = utilsAdapter.getTotalUnreadMessages()
    }

    function clearContactSearchBar() {
        contactSearchBar.clearText()
    }

    function needToChangeToAccount(accountId, index) {
        if (index !== -1) {
            accountComboBox.currentIndex = index
            AccountAdapter.accountChanged(index)
            contactSearchBar.clearText()
        }
    }

    function selectConversationSmartList(accountId, convUid) {
        conversationSmartListViewQmlObjectHolder.selectConversation(accountId,
                                                                    convUid)
    }

    function deselectConversationSmartList() {
        conversationSmartListViewQmlObjectHolder.deselectConversation()
        conversationSmartListView.currentIndex = -1
    }

    // intended -> since strange behavior will happen without this for stackview
    anchors.fill: parent

    ConversationSmartListViewQmlObjectHolder {
        id: conversationSmartListViewQmlObjectHolder

        onShowChatView: {
            conversationSmartListView.needToShowChatView(accountId, convUid)
        }

        onShowConversationTabs: {
            tabBarVisible = visible
            updatePendingRequestCount()
            updateTotalUnreadMessagesCount()
        }
    }

    AccountComboBox {
        id: accountComboBox

        anchors.top: sidePanelRect.top

        width: sidePanelRect.width
        height: 60

        currentIndex: 0

        Connections {
            target: AccountAdapter

            onAccountSignalsReconnect: {
                CallCenter.connectCallstatusChangedSignal(accountId)
                conversationSmartListViewQmlObjectHolder.accountChangedSetUp(
                            accountId)
                sidePanelRect.accountSignalsReconnect(accountId)
            }

            onUpdateConversationForAddedContact: {
                sidePanelRect.needToUpdateConversationForAddedContact()
            }

            onAccountStatusChanged: {
                accountComboBox.updateAccountListModel()
            }
        }

        onAccountChanged: {
            AccountAdapter.accountChanged(index)
            contactSearchBar.clearText()
            contactSearchBar.setPlaceholderString(
                        JamiTheme.contactSearchBarPlaceHolderConversationText)
            pageOne.down = true
            pageTwo.down = false
        }

        onNeedToUpdateSmartList: {
            conversationSmartListView.currentIndex = -1
            conversationSmartListView.updateSmartList(accountId)
        }

        onNeedToBackToWelcomePage: {
            sidePanelRect.accountComboBoxNeedToShowWelcomePage(index)
        }

        Component.onCompleted: {
            AccountAdapter.setQmlObject(accountComboBox)
            AccountAdapter.accountChanged(0)
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

        visible: tabBarVisible

        currentIndex: 0

        onVisibleChanged: {
            if (!tabBarVisible) {
                tabBar.height = 0
                tabBar.anchors.topMargin = 12
            } else {
                tabBar.height = Qt.binding(function () {
                    return Math.max(pageOne.height, pageTwo.height)
                })
                tabBar.anchors.topMargin = 20
            }
        }

        TabButton {
            id: pageOne

            width: tabBar.width / 2 - tabButtonShrinkSize
            height: textConvElement.height + 10

            down: true

            Rectangle {
                id: totalUnreadMessagesCountRect

                anchors.right: pageOne.right
                anchors.rightMargin: 5
                anchors.bottom: pageOne.bottom
                anchors.bottomMargin: pageOne.height - totalUnreadMessagesCountRect.height / 2

                width: 14
                height: 14

                visible: totalUnreadMessagesCount > 0

                Text {
                    id: totalUnreadMessagesCountText

                    anchors.centerIn: totalUnreadMessagesCountRect

                    text: totalUnreadMessagesCount > 9 ? "···" : totalUnreadMessagesCount
                    color: "white"
                }

                radius: 30
                color: JamiTheme.notificationRed
            }

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
                        conversationSmartListViewQmlObjectHolder.setConversationFilter(
                                    "")
                        contactSearchBar.setPlaceholderString(
                                    JamiTheme.contactSearchBarPlaceHolderConversationText)
                        pageOne.down = true
                        pageTwo.down = false
                        setCurrentUidSmartListModelIndex()
                        forceReselectConversationSmartListCurrentIndex()
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

            Rectangle {
                id: pendingRequestCountRect

                anchors.right: pageTwo.right
                anchors.rightMargin: 5
                anchors.bottom: pageTwo.bottom
                anchors.bottomMargin: pageTwo.height - pendingRequestCountRect.height / 2

                width: 14
                height: 14

                visible: pendingRequestCount > 0

                Text {
                    id: pendingRequestCountText

                    anchors.centerIn: pendingRequestCountRect

                    text: pendingRequestCount > 9 ? "···" : pendingRequestCount
                    color: "white"
                }

                radius: 30
                color: JamiTheme.notificationRed
            }

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
                        conversationSmartListViewQmlObjectHolder.setConversationFilter(
                                    "PENDING")
                        contactSearchBar.setPlaceholderString(
                                    JamiTheme.contactSearchBarPlaceHolderInivitionText)
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
        id: sidePanelColumnRect

        anchors.top: tabBar.bottom
        anchors.topMargin: -12

        width: sidePanelRect.width
        height: sidePanelRect.height - accountComboBox.height - tabBar.height

        border.color: JamiTheme.tabbarBorderColor
        radius: 10

        Rectangle {
            id: hideTopBoarderRect

            anchors.top: sidePanelColumnRect.top
            anchors.left: sidePanelColumnRect.left
            anchors.leftMargin: tabBarLeftMargin + 5

            width: pageOne.width + pageTwo.width - 9
            height: 1

            visible: tabBarVisible

            onVisibleChanged: {
                if (!tabBarVisible)
                    sidePanelColumnRect.border.width = 0
                else
                    sidePanelColumnRect.border.width = 1
            }

            color: "white"
        }

        ColumnLayout {
            id: columnLayoutView

            anchors.centerIn: sidePanelColumnRect

            width: sidePanelColumnRect.width - 10
            height: sidePanelColumnRect.height - 20

            ContactSearchBarQmlObjectHolder {
                id: contactSearchBarQmlObjectHolder
            }

            // search bar container to embed search label
            ContactSearchBar {
                id: contactSearchBar

                Layout.alignment: Qt.AlignCenter
                Layout.topMargin: 10
                Layout.rightMargin: 5
                Layout.leftMargin: 5
                Layout.preferredWidth: parent.width - 10
                Layout.preferredHeight: 35

                onContactSearchBarTextChanged: {
                    contactSearchBarQmlObjectHolder.setConversationFilter(text)
                }

                Component.onCompleted: {
                    contactSearchBarQmlObjectHolder.setContactSearchBarQmlObject(
                                contactSearchBar)
                }
            }

            ConversationSmartListView {
                id: conversationSmartListView

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: parent.height - contactSearchBar.height - 30

                onNeedToSelectItems: {
                    conversationSmartListViewQmlObjectHolder.selectConversation(
                                index)
                }

                onNeedToBackToWelcomePage: {
                    sidePanelRect.conversationSmartListViewNeedToShowWelcomePage()
                }

                onNeedToAccessMessageWebView: {
                    sidePanelRect.conversationSmartListNeedToAccessMessageWebView(
                                currentUserDisplayName, currentUserAlias,
                                currentUID, callStackViewShouldShow)
                }

                onNeedToGrabFocus: {
                    contactSearchBar.clearFocus()
                }

                Component.onCompleted: {
                    conversationSmartListViewQmlObjectHolder.setConversationSmartListViewQmlObjectHolder(
                                conversationSmartListView)
                    conversationSmartListView.currentIndex = -1
                }
            }
        }
    }
}
