import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtWebEngine 1.10
import QtWebChannel 1.14

import "../../commoncomponents"

Rectangle {
    id: messageWebViewRect

    property int messageWebViewHeaderPreferredHeight: 60
    property string headerUserAliasLabelText: ""
    property string headerUserUserNameLabelText: ""

    signal needToGoBackToWelcomeView()
    signal setNewMessagesContent(string filePath)

    signal messagesCleared()
    signal messagesLoaded()
    signal pasteKeyDetected()
    signal deleteInteraction(string arg)
    signal retryInteraction(string arg)
    signal openFile(string arg)
    signal acceptFile(string arg)
    signal refuseFile(string arg)
    signal sendMessage(string arg)
    signal sendImage(string arg)
    signal sendFile(string arg)
    signal log(string arg)
    signal acceptInvitation()
    signal refuseInvitation()
    signal blockConversation()
    signal openAudioRecorder(int spikePosX, int spikePosY)
    signal openVideoRecorder(int spikePosX, int spikePosY)
    signal sendMessageContentSaved(string arg)
    signal onComposing(bool isComposing)

    function webViewRunJavaScript(arg) {
        messageWebView.runJavaScript(arg)
    }

    function setSendContactRequestButtonVisible(visible) {
        messageWebViewHeader.sendContactRequestButtonVisible = visible
    }

    function setMessagingHeaderButtonsVisible(visible) {
        messageWebViewHeader.toggleMessagingHeaderButtonsVisible(visible)
    }

    anchors.fill: parent

    JamiFileDialog {
        id: jamiFileDialog

        selectMultiple: true
        selectExisting: true

        onAccepted: {
            var filePaths = jamiFileDialog.fileUrls;
            for(var index = 0; index < filePaths.length; ++ index) {
                // trim file:///
                var trimmedFilePrefixPath = filePaths[index].substr(8)
                messageWebViewRect.setNewMessagesContent(trimmedFilePrefixPath)
            }
        }
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
            messageWebViewRect.needToGoBackToWelcomeView()
        }
    }

    QtObject {
        id: jsBridgeObject

        // ID, under which this object will be known at WebEngineView side
        WebChannel.id: "jsbridge"

        // functions that are exposed, return code can be derived from js side
        // by setting callback function

        function deleteInteraction(arg) {
            messageWebViewRect.deleteInteraction(arg)
            return 0;
        }

        function retryInteraction(arg) {
            messageWebViewRect.retryInteraction(arg)
            return 0;
        }

        function openFile(arg) {
            messageWebViewRect.openFile(arg)
            return 0;
        }

        function acceptFile(arg) {
            messageWebViewRect.acceptFile(arg)
            return 0;
        }

        function refuseFile(arg) {
            messageWebViewRect.refuseFile(arg)
            return 0;
        }

        function sendMessage(arg) {
            messageWebViewRect.sendMessage(arg)
            return 0;
        }

        function sendImage(arg) {
            messageWebViewRect.sendImage(arg)
            return 0;
        }

        function sendFile(arg) {
            messageWebViewRect.sendFile(arg)
            return 0;
        }

        function selectFile() {
            jamiFileDialog.open()
            return 0;
        }

        function log(arg) {
            messageWebViewRect.log(arg)
            return 0;
        }

        function acceptInvitation() {
            messageWebViewRect.acceptInvitation()
            return 0;
        }

        function refuseInvitation() {
            messageWebViewRect.refuseInvitation()
            return 0;
        }

        function blockConversation() {
            messageWebViewRect.blockConversation()
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
            messageWebViewRect.pasteKeyDetected()
            return 0;
        }

        function openAudioRecorder(spikePosX, spikePosY) {
            messageWebViewRect.openAudioRecorder(spikePosX, spikePosY)
            return 0;
        }

        function openVideoRecorder(spikePosX, spikePosY) {
            messageWebViewRect.openVideoRecorder(spikePosX, spikePosY)
            return 0;
        }

        function saveSendMessageContent(arg) {
            messageWebViewRect.sendMessageContentSaved(arg)
            return 0;
        }

        function onComposing(isComposing) {
            messageWebViewRect.onComposing(isComposing)
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
