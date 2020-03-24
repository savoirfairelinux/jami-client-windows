import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.constant.jamitheme 1.0
import net.jami.callcenter 1.0
import net.jami.model.account 1.0
import net.jami.tools.utils 1.0
import net.jami.MessageWebViewQmlObjectHolder 1.0
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
        if(stackOne.depth === 1) {
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
        target: CallCenter

        onShowCallStack: {
            if(callStackView.responsibleAccountId == accountId && callStackView.responsibleConvUid == convUid) {
                if(welcomeViewStack.visible) {
                    welcomeViewStack.pop(null, StackView.Immediate)
                    welcomeViewStack.push(callStackView,StackView.Immediate)
                } else {
                    sidePanelViewStack.pop(null, StackView.Immediate)
                    sidePanelViewStack.push(callStackView, StackView.Immediate)
                }
            }
        }

        onCloseCallStack: {
            if(callStackView.responsibleAccountId == accountId && callStackView.responsibleConvUid == convUid) {
                callStackView.needToCloseInCallConversation()
                if(welcomeViewStack.visible) {
                    welcomeViewStack.pop(null, StackView.Immediate)
                    welcomeViewStack.push(communicationPageMessageWebView, StackView.Immediate)
                } else {
                    sidePanelViewStack.pop(null, StackView.Immediate)
                    sidePanelViewStack.push(communicationPageMessageWebView, StackView.Immediate)
                }
                mainViewWindowSidePanel.deselectConversationSmartList()
            }
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

            callStackView.needToCloseInCallConversation()
            callStackView.responsibleAccountId =  LrcGeneralAdapter.getCurrAccId()
            callStackView.responsibleConvUid = currentUID

            welcomeViewStack.pop(null, StackView.Immediate)
            sidePanelViewStack.pop(null, StackView.Immediate)

            if(sidePanelViewStack.visible && welcomeViewStack.visible) {
                if(inCall) {
                    welcomeViewStack.push(callStackView)
                } else {
                    welcomeViewStack.push(communicationPageMessageWebView)
                }
            } else if(sidePanelViewStack.visible && !welcomeViewStack.visible) {
                if(inCall) {
                    sidePanelViewStack.push(callStackView)
                } else {
                    sidePanelViewStack.push(communicationPageMessageWebView)
                }
            } else if(!sidePanelViewStack.visible && !welcomeViewStack.visible) {
                if(inCall) {
                    sidePanelViewStack.push(callStackView)
                } else {
                    sidePanelViewStack.push(communicationPageMessageWebView)
                }

            }
            // set up chatview
            //console.log(LrcGeneralAdapter.mystruct.val)
            messageWebViewQmlObjectHolder.setupChatView(currentUID)
            callStackView.setCorrspondingMessageWebView(communicationPageMessageWebView)
        }

        onAccountComboBoxNeedToShowWelcomePage: {
            // If the item argument is specified, all items down to (but not including) item will be popped
            welcomeViewStack.pop(welcomePage)
            welcomePage.currentAccountIndex = index
        }

        onConversationSmartListViewNeedToShowWelcomePage: {
            welcomeViewStack.pop(welcomePage)
            welcomePage.currentAccountIndex = 0
        }
    }

    CallStackView {
        id: callStackView

        onAudioCallPageBackButtonIsClicked: {
            if(welcomeViewStack.visible)
                welcomeViewStack.pop(welcomePage)
            else if(sidePanelViewStack.visible)
                sidePanelViewStack.pop(mainViewWindowSidePanel)
        }

        onOutgoingCallPageBackButtonIsClicked: {
            if(welcomeViewStack.visible)
                welcomeViewStack.pop(welcomePage)
            else if(sidePanelViewStack.visible)
                sidePanelViewStack.pop(mainViewWindowSidePanel)
        }
    }

    WelcomePage {
        id: welcomePage
        visible: false
    }

    MessageWebViewQmlObjectHolder {
        id: messageWebViewQmlObjectHolder
    }

    MessageWebView {
        id: communicationPageMessageWebView

        signal toSendMessageContentSaved(string arg)
        signal toMessagesCleared()
        signal toMessagesLoaded()

        visible: false

        onNeedToGoBackToWelcomeView: {
            mainViewWindowSidePanel.deselectConversationSmartList()
            if(communicationPageMessageWebView.visible && !welcomeViewStack.visible) {
                sidePanelViewStack.pop()
            } else if(communicationPageMessageWebView.visible && welcomeViewStack.visible) {
                welcomeViewStack.pop()
            }
        }

        Component.onCompleted: {
            // set qml MessageWebView object pointer to c++
            messageWebViewQmlObjectHolder.setMessageWebViewQmlObject(communicationPageMessageWebView)
        }
    }

    SplitView  {
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
        if(mainViewWindow.width < sidePanelViewStackPreferedWidth + welcomePageGroupPreferedWidth - 5 && welcomeViewStack.visible) {
            welcomeViewStack.visible = false

            //The find callback function is called for each item in the stack
            var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                return index > 0
            })
            if(inWelcomeViewStack) {
                recursionStackViewItemMove(welcomeViewStack, sidePanelViewStack)
                //sidePanelViewStack.push(welcomeViewStack.pop(StackView.Immediate), StackView.Immediate)
                //welcomeViewStack.pop(null)
            }

            sidePanelViewStack.maximumWidth = splitView.width

            mainViewWindow.update()
        } else if(mainViewWindow.width >= sidePanelViewStackPreferedWidth + welcomePageGroupPreferedWidth + 5 && !welcomeViewStack.visible) {
            welcomeViewStack.visible = true

            var inSidePanelViewStack = sidePanelViewStack.find(function(item, index) {
                return index > 0
            })
            if(inSidePanelViewStack) {
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

    Component.onCompleted: {
        CallCenter.init()
    }
}
