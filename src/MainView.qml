import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import Qt.model.account 1.0
import Qt.constant.color 1.0
import Qt.tools.utils 1.0

Window {
    id: mainViewWindow

    property int minWidth: 320
    property int minHeight: aboutPopUpDialog.contentHeight
    property int textFontSize: 9

    property int mainViewWindowPreferedWidth: 650
    property int mainViewWindowPreferedHeight: 600
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

    QmlUtils {
        id: qmlUtils
    }

    SidePanel {
        id: mainViewWindowSidePanel
    }

    WelcomePage {
        id: welcomePage
    }

    /*Component {
        id: popback
        Rectangle {
            Button {
                id: backButton

                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                text: "Back"
                onClicked: {
                    var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                        return index > 0
                    })
                    var inCallViewStack = callViewStack.find(function(item, index) {
                        return index > 0
                    })
                    if(inWelcomeViewStack)
                        welcomeViewStack.pop()
                    else
                        callViewStack.pop()
                }
            }

            Text {
                anchors.left: backButton.right
                anchors.leftMargin: 10
                anchors.verticalCenter: backButton.verticalCenter
                fontSizeMode: Text.Fit

                font.pointSize: 10
                text: userTypeName
            }
        }
    }*/

    Rectangle {
        id: splitView

        anchors.fill: parent
        width: mainViewWindow.width
        height: mainViewWindow.height

        StackView {
            id: callViewStack

            initialItem: mainViewWindowSidePanel
            anchors.left: splitView.left
            width: splitView.width / 2
            height: splitView.height

            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
        }

        StackView {
            id: welcomeViewStack

            initialItem: welcomePage
            anchors.left: callViewStack.right
            width: splitView.width / 2
            height: splitView.height

            popEnter: Transition {
                PropertyAnimation {
                    property: "opacity"
                    from: 0
                    to:1
                    duration: 200
                }
            }
        }
    }

    onWidthChanged: {
        if(mainViewWindow.width < mainViewWindowPreferedWidth / 2 + welcomePageGroupPreferedWidth){
            welcomeViewStack.visible = false
            callViewStack.width = mainViewWindow.width

            var inWelcomeViewStack = welcomeViewStack.find(function(item, index) {
                return index > 0
            })
            if(inWelcomeViewStack) {
                callViewStack.push(popback)
                welcomeViewStack.pop()
            }
        } else if(mainViewWindow.width >= 500) {
            var inCallViewStack = callViewStack.find(function(item, index) {
                return index > 0
            })
            if(inCallViewStack) {
                welcomeViewStack.push(popback)
                callViewStack.pop()
            }

            welcomeViewStack.visible = true
            callViewStack.width = mainViewWindow.width / 2
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
            source: "images/jami.png"
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
