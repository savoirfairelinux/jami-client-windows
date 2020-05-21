
/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0
import net.jami.AccountAdapter 1.0
import net.jami.ConversationsAdapter 1.0
import net.jami.CallAdapter 1.0

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


    /*
     * Hack -> force redraw.
     */
    function forceReselectConversationSmartListCurrentIndex() {
        var index = conversationSmartListView.currentIndex
        conversationSmartListView.currentIndex = -1
        conversationSmartListView.currentIndex = index
    }


    /*
     * For contact request conv to be focused correctly.
     */
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

    function deselectConversationSmartList() {
        ConversationsAdapter.deselectConversation()
        conversationSmartListView.currentIndex = -1
    }

    function forceUpdateConversationSmartListView() {
        conversationSmartListView.updateConversationSmartListView()
    }


    /*
     * Intended -> since strange behavior will happen without this for stackview.
     */
    anchors.fill: parent

    AccountComboBox {
        id: accountComboBox

        anchors.top: sidePanelRect.top

        width: sidePanelRect.width
        height: 60

        currentIndex: 0

        Connections {
            target: AccountAdapter

            onAccountSignalsReconnect: {
                CallAdapter.connectCallstatusChangedSignal(accountId)
                ConversationsAdapter.accountChangedSetUp(accountId)
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
            sidePanelTabBar.converstationTabDown = true
            sidePanelTabBar.invitationTabDown = false
        }

        onNeedToUpdateSmartList: {
            conversationSmartListView.currentIndex = -1
            conversationSmartListView.updateSmartList(accountId)
        }

        onNeedToBackToWelcomePage: {
            sidePanelRect.accountComboBoxNeedToShowWelcomePage(index)
        }

        Component.onCompleted: {
            AccountAdapter.setQmlObject(this)
            AccountAdapter.accountChanged(0)
        }
    }

    SidePanelTabBar {
        id: sidePanelTabBar

        anchors.top: accountComboBox.bottom
        anchors.topMargin: 20
        anchors.left: sidePanelRect.left
        anchors.leftMargin: tabBarLeftMargin

        width: sidePanelRect.width
        height: Math.max(sidePanelTabBar.converstationTabHeight,
                         sidePanelTabBar.invitationTabHeight)
    }

    Rectangle {
        id: sidePanelColumnRect

        anchors.top: sidePanelTabBar.bottom
        anchors.topMargin: -12

        width: sidePanelRect.width
        height: sidePanelRect.height - accountComboBox.height - sidePanelTabBar.height

        border.color: JamiTheme.tabbarBorderColor
        radius: 10

        Rectangle {
            id: hideTopBoarderRect

            anchors.top: sidePanelColumnRect.top
            anchors.left: sidePanelColumnRect.left
            anchors.leftMargin: tabBarLeftMargin + 5

            width: sidePanelTabBar.converstationTabWidth + sidePanelTabBar.invitationTabWidth - 9
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

            width: sidePanelColumnRect.width
            height: sidePanelColumnRect.height - 20


            /*
             * Search bar container to embed search label
             */
            ContactSearchBar {
                id: contactSearchBar

                Layout.alignment: Qt.AlignCenter
                Layout.topMargin: 10
                Layout.rightMargin: 5
                Layout.leftMargin: 5
                Layout.preferredWidth: parent.width - 10
                Layout.preferredHeight: 35

                onContactSearchBarTextChanged: {
                    utilsAdapter.setConversationFilter(text)
                }
            }

            ConversationSmartListView {
                id: conversationSmartListView

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: parent.height - contactSearchBar.height - 30

                Connections {
                    target: ConversationsAdapter

                    onShowChatView: {
                        conversationSmartListView.needToShowChatView(accountId,
                                                                     convUid)
                    }

                    onShowConversationTabs: {
                        tabBarVisible = visible
                        updatePendingRequestCount()
                        updateTotalUnreadMessagesCount()
                    }
                }

                onNeedToSelectItems: {
                    ConversationsAdapter.selectConversation(index)
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
                    ConversationsAdapter.setQmlObject(this)
                    conversationSmartListView.currentIndex = -1
                }
            }
        }
    }
}
