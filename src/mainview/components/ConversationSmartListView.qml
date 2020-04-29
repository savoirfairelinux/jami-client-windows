import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0
import net.jami.CallAdapter 1.0

ListView {
    id: conversationSmartListView

    signal needToAccessMessageWebView(string currentUserDisplayName, string currentUserAlias, string currentUID, bool callStackViewShouldShow)
    signal needToSelectItems(int index)
    signal needToDeselectItems
    signal needToBackToWelcomePage
    signal needToGrabFocus

    signal needToShowChatView(string accountId, string convUid)
    signal currentIndexIsChanged
    signal forceUpdatePotentialInvalidItem

    function modelSorted(contactURIToCompare) {
        var conversationSmartListViewModel = conversationSmartListView.model
        conversationSmartListView.currentIndex = -1
        updateConversationSmartListView()
        for (var i = 0; i < count; i++) {
            if (conversationSmartListViewModel.data(
                        conversationSmartListViewModel.index(i, 0),
                        261) === contactURIToCompare) {
                conversationSmartListView.currentIndex = i
                break
            }
        }
    }

    function updateConversationSmartListView() {
        var conversationSmartListViewModel = conversationSmartListView.model
        conversationSmartListViewModel.dataChanged(
                    conversationSmartListViewModel.index(0, 0),
                    conversationSmartListViewModel.index(
                        conversationSmartListViewModel.rowCount() - 1, 0))
        conversationSmartListView.forceUpdatePotentialInvalidItem()
    }

    function setModel(model) {
        conversationSmartListView.model = model
    }

    function backToWelcomePage() {
        conversationSmartListView.needToBackToWelcomePage()
    }

    function updateSmartList(accountId) {
        conversationSmartListView.model.setAccount(accountId)
    }

    Connections {
        target: CallAdapter

        onUpdateConversationSmartList: {
            updateConversationSmartListView()
        }
    }

    onCurrentIndexChanged: {
        conversationSmartListView.currentIndexIsChanged()
    }

    clip: true

    delegate: ConversationSmartListViewItemDelegate {
        id: smartListItemDelegate
    }

    ScrollIndicator.vertical: ScrollIndicator {}
}
