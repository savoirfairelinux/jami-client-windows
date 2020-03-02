import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtWebEngine 1.10

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

    WebEngineView {
        id: messageWebView

        anchors.top: messageWebViewHeader.bottom
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: messageWebViewRect.height - messageWebViewHeaderPreferredHeight

        url: "https://www.qt.io"
        settings.javascriptEnabled: true
        settings.javascriptCanOpenWindows: true
        settings.fullScreenSupportEnabled: true
        settings.allowRunningInsecureContent: true
        settings.localContentCanAccessRemoteUrls: true
        settings.localContentCanAccessFileUrls: true
        settings.errorPageEnabled: false
        settings.pluginsEnabled: false
        settings.screenCaptureEnabled: false
        settings.linksIncludedInFocusChain: false
        settings.localStorageEnabled: false

        Component.onCompleted: {
            //utilsAdapter.Q
        }
    }

    color: "pink"
}
