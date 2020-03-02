import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Rectangle {
    id: communicationPage

    signal needToBackToWelcomeView()

    anchors.fill: parent

    CallStack {
        id: communicationPageCallStack

        anchors.left: communicationPage.left

        width: communicationPage.width / 2
        height: communicationPage.height
    }

    MessageWebView {
        id: communicationPageMessageWebView

        anchors.left: communicationPageCallStack.right

        width: communicationPage.width / 2
        height: communicationPage.height

        onBackToWelcomeViewButtonClicked: {
            //if(communicationPageCallStack.visible)
            communicationPage.needToBackToWelcomeView()
        }
    }
}
