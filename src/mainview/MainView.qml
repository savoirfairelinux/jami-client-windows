import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.constant.jamitheme 1.0
import net.jami.model.account 1.0
import net.jami.tools.utils 1.0
import net.jami.MessageWebViewQmlObjectHolder 1.0

// import qml component files
import "components"

Window {
    id: mainViewWindow

    property int minWidth: callViewStackPreferedWidth
    property int minHeight: aboutPopUpDialog.contentHeight
    property int textFontSize: 9

    property int mainViewWindowPreferedWidth: 650
    property int mainViewWindowPreferedHeight: 600
    property int splitViewHandlePreferedWidth: 4
    property int callViewStackPreferedWidth: 250
    property int welcomePageGroupPreferedWidth: 250
    property int aboutPopUpPreferedWidth: 250

    // To calculate tab bar bottom border hidden rect left margin
    property int tabBarLeftMargin: 8
    property int tabButtonShrinkSize: 8

    title: "Jami"
    visible: true
    width: mainViewWindowPreferedWidth
    height: mainViewWindowPreferedHeight
    minimumWidth: minWidth
    minimumHeight: minHeight

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

            if(!communicationPageMessageWebView.visible && !welcomeViewStack.visible) {
                callViewStack.push(communicationPageMessageWebView)
            } else if(!communicationPageMessageWebView.visible && welcomeViewStack.visible) {
                welcomeViewStack.push(communicationPageMessageWebView)
            }

            // set up chatview
            messageWebViewQmlObjectHolder.setupChatView(currentUID)
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
            if(communicationPageMessageWebView.visible && !welcomeViewStack.visible) {
                callViewStack.pop()
            } else if(communicationPageMessageWebView.visible && welcomeViewStack.visible) {
                welcomeViewStack.pop()
            }
        }

        onSendMessageContentSaved: {
            communicationPageMessageWebView.toSendMessageContentSaved(arg)
        }

        onMessagesCleared: {
            communicationPageMessageWebView.toMessagesCleared()
        }

        onMessagesLoaded: {
            communicationPageMessageWebView.toMessagesLoaded()
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
                implicitWidth: splitViewHandlePreferedWidth
                implicitHeight: splitView.height
                color: SplitHandle.pressed ? JamiTheme.pressColor : (SplitHandle.hovered ? JamiTheme.hoverColor : "grey")
        }

        StackView {
            id: callViewStack

            property int maximumWidth: callViewStackPreferedWidth + 100

            initialItem: mainViewWindowSidePanel

            SplitView.minimumWidth: callViewStackPreferedWidth
            SplitView.maximumWidth: maximumWidth
            SplitView.fillHeight: true

            clip: true
        }

        StackView {
            id: welcomeViewStack

            initialItem: welcomePage

            SplitView.maximumWidth: splitView.width - callViewStack.width
            SplitView.fillHeight: true

            clip: true
        }
    }

    onWidthChanged: {
        if(mainViewWindow.width < callViewStackPreferedWidth + welcomePageGroupPreferedWidth - 5 && welcomeViewStack.visible) {
            welcomeViewStack.visible = false

            //The find callback function is called for each item in the stack
            var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                return index > 0
            })
            if(inWelcomeViewStack) {
                callViewStack.push(welcomeViewStack.pop(StackView.Immediate), StackView.Immediate)
            }

            callViewStack.maximumWidth = splitView.width
            mainViewWindow.update()
        } else if(mainViewWindow.width >= callViewStackPreferedWidth + welcomePageGroupPreferedWidth + 5 && !welcomeViewStack.visible) {
            welcomeViewStack.visible = true

            var inCallViewStack = callViewStack.find(function(item, index) {
                return index > 0
            })
            if(inCallViewStack) {
                welcomeViewStack.push(callViewStack.pop(StackView.Immediate), StackView.Immediate)
            }

            callViewStack.maximumWidth = callViewStackPreferedWidth + 100
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
}
