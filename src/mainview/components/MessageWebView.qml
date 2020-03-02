import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtWebEngine 1.10
import QtWebChannel 1.14

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
            //jsBridgeObject.someSignal()
            messageWebViewRect.needToGoBackToWelcomeView()
        }
    }

    QtObject {
        id: jsBridgeObject

        // ID, under which this object will be known at WebEngineView side
        WebChannel.id: "jsbridge"

        signal someSignal();
    }

    WebEngineView {
        id: messageWebView

        anchors.top: messageWebViewHeader.bottom
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: messageWebViewRect.height - messageWebViewHeaderPreferredHeight

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

        webChannel: messageWebViewChannel
        profile: messageWebViewProfile

        //qrc:///qtwebchannel/qwebchannel.js
        onLoadingChanged: {
            if (loadRequest.status == WebEngineView.LoadSucceededStatus) {
                messageWebView.runJavaScript(utilsAdapter.getStyleSheet("chatcss", utilsAdapter.qStringFromFile(":/chatview.css")))
                messageWebView.runJavaScript(utilsAdapter.getStyleSheet("chatwin", utilsAdapter.qStringFromFile(":/chatview-windows.css")))

                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/jed.js"))
                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/linkify.js"))
                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/linkify-html.js"))
                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/linkify-string.js"))
                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/qwebchannel.js"))
                messageWebView.runJavaScript(utilsAdapter.qStringFromFile(":/chatview.js"))
                messageWebView.runJavaScript("init_i18n();")
                messageWebView.runJavaScript("displayNavbar(false);")
            }
        }
        Component.onCompleted: {
            messageWebView.loadHtml(utilsAdapter.qStringFromFile(":/chatview.html"), ":/chatview.html")
        }
    }

    WebEngineProfile {
        id: messageWebViewProfile

        cachePath: utilsAdapter.getCachePath()
        persistentStoragePath: utilsAdapter.getCachePath()
        persistentCookiesPolicy: WebEngineProfile.NoPersistentCookies
        httpCacheType: WebEngineProfile.NoCache
        httpUserAgent: "jami-windows"
    }

    WebChannel {
        id: messageWebViewChannel
        registeredObjects: [jsBridgeObject]
    }

    color: "pink"
}
