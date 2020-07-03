
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
import net.jami.Models 1.0

import "../../commoncomponents"

Rectangle {
    id: sidePanelRect

    property bool tabBarVisible: true
    property int pendingRequestCount: 0
    property int totalUnreadMessagesCount: 0

    signal conversationSmartListNeedToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias, string currentUID, bool callStackViewShouldShow, bool isAudioOnly, string callStateStr)
    signal accountComboBoxNeedToShowWelcomePage(int index)
    signal conversationSmartListViewNeedToShowWelcomePage
    signal accountSignalsReconnect(string accountId)
    signal needToUpdateConversationForAddedContact
    signal needToAddNewAccount
    signal settingBtnClicked_AccountComboBox


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
        pendingRequestCount = UtilsAdapter.getTotalPendingRequest()
    }

    function updateTotalUnreadMessagesCount() {
        totalUnreadMessagesCount = UtilsAdapter.getTotalUnreadMessages()
    }

    function clearContactSearchBar() {
        contactSearchBar.clearText()
    }

    function accountChangedUIReset() {
        contactSearchBar.clearText()
        contactSearchBar.setPlaceholderString(
                    JamiTheme.contactSearchBarPlaceHolderConversationText)
        sidePanelTabBar.converstationTabDown = true
        sidePanelTabBar.invitationTabDown = false
    }

    function needToChangeToAccount(accountId, index) {
        if (index !== -1) {
            accountComboBox.currentIndex = index
            AccountAdapter.accountChanged(index)
            accountChangedUIReset()
        }
    }

    function refreshAccountComboBox(index) {
        accountComboBox.resetAccountListModel()


        /*
         * To make sure that the ui is refreshed for accountComboBox.
         * Note: index here should always be zero.
         */
        if (accountComboBox.currentIndex === 0)
            accountComboBox.currentIndex = 1
        accountComboBox.currentIndex = index
        accountComboBox.updateAccountListModel()
        AccountAdapter.accountChanged(index)
        accountChangedUIReset()
    }

    function accountDeletion() {
        refreshAccountComboBox(0)
    }

    function newAccountAdded(index) {
        refreshAccountComboBox(index)
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

            function onAccountSignalsReconnect(accountId) {
                CallAdapter.connectCallStatusChanged(accountId)
                ConversationsAdapter.accountChangedSetUp(accountId)
                sidePanelRect.accountSignalsReconnect(accountId)
            }

            function onUpdateConversationForAddedContact() {
                sidePanelRect.needToUpdateConversationForAddedContact()
            }

            function onAccountStatusChanged() {
                accountComboBox.updateAccountListModel()
            }
        }

        onSettingBtnClicked:{
            settingBtnClicked_AccountComboBox()
        }

        onAccountChanged: {
            AccountAdapter.accountChanged(index)
            accountChangedUIReset()
        }

        onNeedToUpdateSmartList: {
            conversationSmartListView.currentIndex = -1
            conversationSmartListView.updateSmartList(accountId)
        }

        onNeedToBackToWelcomePage: {
            sidePanelRect.accountComboBoxNeedToShowWelcomePage(index)
        }

        onNewAccountButtonClicked: {
            sidePanelRect.needToAddNewAccount()
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
                    UtilsAdapter.setConversationFilter(text)
                }
            }

            ConversationSmartListView {
                id: conversationSmartListView

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: parent.height - contactSearchBar.height - 30

                Connections {
                    target: ConversationsAdapter

                    function onShowChatView(accountId, convUid) {
                        conversationSmartListView.needToShowChatView(accountId,
                                                                     convUid)
                    }

                    function onShowConversationTabs(visible) {
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
                                currentUID, callStackViewShouldShow,
                                isAudioOnly, callStateStr)
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
