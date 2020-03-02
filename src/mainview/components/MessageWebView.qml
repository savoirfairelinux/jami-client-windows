import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14

Rectangle {
    id: messageWebViewRect

    anchors.fill: parent

    property int messageWebViewHeaderPreferredHeight: 60

    signal needToGoBackToWelcomeView()

    MessageWebViewHeader {
        id: messageWebViewHeader

        anchors.top: messageWebViewRect.top
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: messageWebViewHeaderPreferredHeight

        onBackToWelcomeViewButtonClicked: {
            messageWebViewRect.needToGoBackToWelcomeView()
        }
    }

    color: "pink"
}