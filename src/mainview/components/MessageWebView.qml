import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtWebEngine 1.10
import QtWebChannel 1.14
import net.jami.LrcGeneralAdapter 1.0

Rectangle {
    id: messageWebViewRect

    property int messageWebViewHeaderPreferredHeight: 60
    property string headerUserAliasLabelText: ""
    property string headerUserUserNameLabelText: ""

    signal needToGoBackToWelcomeView()
    signal sendMessageContentSaved(string arg)
    signal messagesCleared()
    signal messagesLoaded()

    function setMessagesVisibility(visible) {
        messageWebView.runJavaScript(visible ? "showMessagesDiv();" : "hideMessagesDiv();")
    }

    function requestSendMessageContent() {
        messageWebView.runJavaScript("requestSendMessageContent();")
    }

    function setInvitation(show, contactUri, contactId) {
        var s = show ? "showInvitation(\"%1\", \"%2\")".arg(contactUri).arg(contactId) : "showInvitation()";
        messageWebView.runJavaScript(s)
    }

    function clear() {
        messageWebView.runJavaScript("clearMessages();")
    }

    function printHistory(interactionsStr) {
        messageWebView.runJavaScript("printHistory(%1);".arg(interactionsStr))
    }

    MessageWebViewHeader {
        id: messageWebViewHeader

        anchors.top: messageWebViewRect.top
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: messageWebViewHeaderPreferredHeight

        userAliasLabelText: headerUserAliasLabelText
        userUserNameLabelText: headerUserUserNameLabelText

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

        // functions that are exposed, return code can be derived from js side
        // by setting callback function

        function deleteInteraction(arg) {
            return 0;
        }

        function retryInteraction(arg) {
            return 0;
        }

        function openFile(arg) {
            return 0;
        }

        function acceptFile(arg) {
            return 0;
        }

        function refuseFile(arg) {
            return 0;
        }

        function sendMessage(arg) {
            return 0;
        }

        function sendImage(arg) {
            return 0;
        }

        function sendFile(arg) {
            return 0;
        }

        function selectFile() {
            return 0;
        }

        function log(arg) {
            return 0;
        }

        function acceptInvitation() {
            return 0;
        }

        function refuseInvitation() {
            return 0;
        }

        function blockConversation() {
            return 0;
        }

        function emitMessagesCleared() {
            messageWebViewRect.messagesCleared()
            return 0;
        }

        function emitMessagesLoaded() {
            messageWebViewRect.messagesLoaded()
            return 0;
        }

        function emitPasteKeyDetected() {
            return 0;
        }

        function openAudioRecorder(spikePosX, spikePosY) {
            return 0;
        }

        function openVideoRecorder(spikePosX, spikePosY) {
            return 0;
        }

        function saveSendMessageContent(arg) {
            messageWebViewRect.sendMessageContentSaved(arg)
            return 0;
        }

        function onComposing(isComposing) {
            return 0;
        }

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
