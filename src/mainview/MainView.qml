import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.JamiTheme 1.0
import net.jami.CallAdapter 1.0
import net.jami.AccountListModel 1.0
import net.jami.UtilsAdapter 1.0
import net.jami.MessagesAdapter 1.0
import net.jami.LrcGeneralAdapter 1.0

// import qml component files
import "components"

Window {
    id: mainViewWindow

    property int minWidth: sidePanelViewStackPreferedWidth
    property int minHeight: aboutPopUpDialog.contentHeight

    property int mainViewWindowPreferedWidth: 650
    property int mainViewWindowPreferedHeight: 600
    property int sidePanelViewStackPreferedWidth: 250
    property int welcomePageGroupPreferedWidth: 250
    property int aboutPopUpPreferedWidth: 250

    // To calculate tab bar bottom border hidden rect left margin
    property int tabBarLeftMargin: 8
    property int tabButtonShrinkSize: 8

    function recursionStackViewItemMove(stackOne, stackTwo) {
        // move all items (expect the bottom item) to stacktwo by the same order in stackone
        if (stackOne.depth === 1) {
            return
        }

        var tempItem = stackOne.pop(StackView.Immediate)
        recursionStackViewItemMove(stackOne, stackTwo)
        stackTwo.push(tempItem, StackView.Immediate)
    }

    title: "Jami"
    visible: true
    width: mainViewWindowPreferedWidth
    height: mainViewWindowPreferedHeight
    minimumWidth: minWidth
    minimumHeight: minHeight

    Connections {
        target: CallAdapter

        onShowCallStack: {
            if (callStackView.responsibleAccountId == accountId
                    && callStackView.responsibleConvUid == convUid) {
                if (welcomeViewStack.visible) {
                    welcomeViewStack.pop(null, StackView.Immediate)
                    welcomeViewStack.push(callStackView, StackView.Immediate)
                } else {
                    sidePanelViewStack.pop(null, StackView.Immediate)
                    sidePanelViewStack.push(callStackView, StackView.Immediate)
                }
            }
        }

        onCloseCallStack: {
            if (callStackView.responsibleAccountId == accountId
                    && callStackView.responsibleConvUid == convUid) {
                if (welcomeViewStack.find(function (item, index) {
                    return item.objectName === "callStackViewObject"
                }) || sidePanelViewStack.find(function (item, index) {
                    return item.objectName === "callStackViewObject"
                })) {
                    callStackView.needToCloseInCallConversation()
                    if (welcomeViewStack.visible) {
                        welcomeViewStack.pop(null, StackView.Immediate)
                        welcomeViewStack.push(communicationPageMessageWebView,
                                              StackView.Immediate)
                    } else {
                        sidePanelViewStack.pop(null, StackView.Immediate)
                        sidePanelViewStack.push(
                                    communicationPageMessageWebView,
                                    StackView.Immediate)
                    }
                }
            }
        }

        onIncomingCallNeedToSetupMainView: {
            welcomeViewStack.pop(null, StackView.Immediate)
            sidePanelViewStack.pop(null, StackView.Immediate)

            var index = LrcGeneralAdapter.getCurrAccList().indexOf(accountId)
            var name = utilsAdapter.getBestName(accountId, convUid)
            var id = utilsAdapter.getBestId(accountId, convUid)

            communicationPageMessageWebView.headerUserAliasLabelText = name
            communicationPageMessageWebView.headerUserUserNameLabelText = (name !== id) ? id : ""

            callStackView.needToCloseInCallConversation()
            callStackView.setCorrspondingMessageWebView(
                        communicationPageMessageWebView)

            callStackView.responsibleAccountId = accountId
            callStackView.responsibleConvUid = convUid
            callStackView.updateCorrspondingUI()

            mainViewWindowSidePanel.needToChangeToAccount(accountId, index)
            mainViewWindowSidePanel.selectConversationSmartList(accountId,
                                                                convUid)
            MessagesAdapter.setupChatView(convUid)
        }
    }

    AccountListModel {
        id: accountListModel
    }

    UtilsAdapter {
        id: utilsAdapter
    }

    SidePanel {
        id: mainViewWindowSidePanel

        onConversationSmartListNeedToAccessMessageWebView: {

            communicationPageMessageWebView.headerUserAliasLabelText = currentUserAlias
            communicationPageMessageWebView.headerUserUserNameLabelText = currentUserDisplayName

            if (callStackViewShouldShow) {
                LrcGeneralAdapter.setCurrentCall(LrcGeneralAdapter.getCurrAccId(
                                                     ), currentUID)
            }
            callStackView.needToCloseInCallConversation()
            callStackView.responsibleAccountId = LrcGeneralAdapter.getCurrAccId(
                        )
            callStackView.responsibleConvUid = currentUID
            callStackView.updateCorrspondingUI()

            // set up chatview
            MessagesAdapter.setupChatView(currentUID)
            callStackView.setCorrspondingMessageWebView(
                        communicationPageMessageWebView)

            if (welcomeViewStack.find(function (item, index) {
                return item.objectName === "communicationPageMessageWebView"
            }) || sidePanelViewStack.find(function (item, index) {
                return item.objectName === "communicationPageMessageWebView"
            })) {
                if (!callStackViewShouldShow)
                    return
            }

            welcomeViewStack.pop(null, StackView.Immediate)
            sidePanelViewStack.pop(null, StackView.Immediate)

            if (sidePanelViewStack.visible && welcomeViewStack.visible) {
                if (callStackViewShouldShow) {
                    welcomeViewStack.push(callStackView)
                } else {
                    welcomeViewStack.push(communicationPageMessageWebView)
                }
            } else if (sidePanelViewStack.visible
                       && !welcomeViewStack.visible) {
                if (callStackViewShouldShow) {
                    sidePanelViewStack.push(callStackView)
                } else {
                    sidePanelViewStack.push(communicationPageMessageWebView)
                }
            } else if (!sidePanelViewStack.visible
                       && !welcomeViewStack.visible) {
                if (callStackViewShouldShow) {
                    sidePanelViewStack.push(callStackView)
                } else {
                    sidePanelViewStack.push(communicationPageMessageWebView)
                }
            }
        }

        onAccountComboBoxNeedToShowWelcomePage: {
            // If the item argument is specified, all items down to (but not including) item will be popped
            welcomeViewStack.pop(welcomePage)
            welcomePage.currentAccountIndex = index
            qrDialog.currentAccountIndex = index
        }

        onConversationSmartListViewNeedToShowWelcomePage: {
            welcomeViewStack.pop(welcomePage)
            welcomePage.currentAccountIndex = 0
            qrDialog.currentAccountIndex = 0
        }

        onAccountSignalsReconnect: {
            MessagesAdapter.accountChangedSetUp(accountId)
        }

        onNeedToUpdateConversationForAddedContact: {
            MessagesAdapter.updateConversationForAddedContact()
            mainViewWindowSidePanel.clearContactSearchBar()
            mainViewWindowSidePanel.forceReselectConversationSmartListCurrentIndex()
        }
    }

    CallStackView {
        id: callStackView

        objectName: "callStackViewObject"

        onCallPageBackButtonIsClicked: {
            mainViewWindowSidePanel.deselectConversationSmartList()
            if (welcomeViewStack.visible)
                welcomeViewStack.pop(welcomePage)
            else if (sidePanelViewStack.visible)
                sidePanelViewStack.pop(mainViewWindowSidePanel)
        }

        onOutgoingCallPageBackButtonIsClicked: {
            mainViewWindowSidePanel.deselectConversationSmartList()
            if (welcomeViewStack.visible)
                welcomeViewStack.pop(welcomePage)
            else if (sidePanelViewStack.visible)
                sidePanelViewStack.pop(mainViewWindowSidePanel)
        }
    }

    WelcomePage {
        id: welcomePage
        visible: false
    }

    MessageWebView {
        id: communicationPageMessageWebView

        objectName: "communicationPageMessageWebView"

        signal toSendMessageContentSaved(string arg)
        signal toMessagesCleared
        signal toMessagesLoaded

        visible: false

        onNeedToGoBackToWelcomeView: {
            mainViewWindowSidePanel.deselectConversationSmartList()
            if (communicationPageMessageWebView.visible
                    && !welcomeViewStack.visible) {
                sidePanelViewStack.pop()
            } else if (communicationPageMessageWebView.visible
                       && welcomeViewStack.visible) {
                welcomeViewStack.pop()
            }
        }

        onNeedToSendContactRequest: {
            MessagesAdapter.sendContactRequest()
        }

        onAcceptInvitation: {
            MessagesAdapter.acceptInvitation()
        }

        onRefuseInvitation: {
            MessagesAdapter.refuseInvitation()
        }

        onBlockConversation: {
            MessagesAdapter.blockConversation()
        }

        Component.onCompleted: {
            // set qml MessageWebView object pointer to c++
            MessagesAdapter.setQmlObject(this)
        }
    }

    SplitView {
        id: splitView

        anchors.fill: parent
        width: mainViewWindow.width
        height: mainViewWindow.height

        handle: Rectangle {
            implicitWidth: JamiTheme.splitViewHandlePreferedWidth
            implicitHeight: splitView.height
            color: SplitHandle.pressed ? JamiTheme.pressColor : (SplitHandle.hovered ? JamiTheme.hoverColor : JamiTheme.tabbarBorderColor)
        }

        StackView {
            id: sidePanelViewStack

            property int maximumWidth: sidePanelViewStackPreferedWidth + 100

            initialItem: mainViewWindowSidePanel

            SplitView.minimumWidth: sidePanelViewStackPreferedWidth
            SplitView.maximumWidth: maximumWidth
            SplitView.fillHeight: true

            clip: true
        }

        StackView {
            id: welcomeViewStack

            initialItem: welcomePage

            SplitView.maximumWidth: splitView.width - sidePanelViewStack.width
            SplitView.fillHeight: true

            clip: true
        }
    }

    onWidthChanged: {
        if (mainViewWindow.width < sidePanelViewStackPreferedWidth
                + welcomePageGroupPreferedWidth - 5
                && welcomeViewStack.visible) {
            welcomeViewStack.visible = false

            //The find callback function is called for each item in the stack
            var inWelcomeViewStack = welcomeViewStack.find(
                        function (item, index) {
                            return index > 0
                        })
            if (inWelcomeViewStack) {
                recursionStackViewItemMove(welcomeViewStack, sidePanelViewStack)
                //sidePanelViewStack.push(welcomeViewStack.pop(StackView.Immediate), StackView.Immediate)
                //welcomeViewStack.pop(null)
            }

            sidePanelViewStack.maximumWidth = splitView.width

            mainViewWindow.update()
        } else if (mainViewWindow.width >= sidePanelViewStackPreferedWidth
                   + welcomePageGroupPreferedWidth + 5
                   && !welcomeViewStack.visible) {
            welcomeViewStack.visible = true

            var inSidePanelViewStack = sidePanelViewStack.find(
                        function (item, index) {
                            return index > 0
                        })
            if (inSidePanelViewStack) {
                recursionStackViewItemMove(sidePanelViewStack, welcomeViewStack)
                //welcomeViewStack.push(sidePanelViewStack.pop(StackView.Immediate), StackView.Immediate)
                //sidePanelViewStack.pop(null)
            }

            sidePanelViewStack.maximumWidth = sidePanelViewStackPreferedWidth + 100

            mainViewWindow.update()
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
            source: "qrc:/images/jami.png"
        }
    }

    AboutPopUp {
        id: aboutPopUpDialog

        x: Math.round((mainViewWindow.width - width) / 2)
        y: Math.round((mainViewWindow.height - height) / 2)
        width: Math.max(mainViewWindow.width / 2, aboutPopUpPreferedWidth)
        height: aboutPopUpDialog.contentHeight
    }

    WelcomePageQrDialog {
        id: qrDialog

        x: Math.round((mainViewWindow.width - width) / 2)
        y: Math.round((mainViewWindow.height - height) / 2)
        width: qrDialog.contentHeight
        height: qrDialog.contentHeight
    }

    Component.onCompleted: {
        CallAdapter.initQmlObject()
    }
}
