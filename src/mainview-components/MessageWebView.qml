import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

Rectangle {
    id: messageWebViewRect

    anchors.fill: parent

    signal needToGoBackToWelcomeView()

    MessageWebViewHeader {
        id: messageWebViewHeader

        anchors.top: messageWebViewRect.top
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: 60

        onBackToWelcomeViewButtonClicked: {
            messageWebViewRect.needToGoBackToWelcomeView()
        }
    }

    color: "pink"
}
