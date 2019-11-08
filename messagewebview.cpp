/***************************************************************************
 * Copyright (C) 2017-2019 by Savoir-faire Linux                           *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Alexandre Viau <alexandre.viau@savoirfairelinux.com>            *
 * Author: Sébastien Blin <sebastien.blin@savoirfairelinux.com>            *
 * Author: Hugo Lefeuvre <hugo.lefeuvre@savoirfairelinux.com>              *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 **************************************************************************/

#include "messagewebview.h"
#include "recordwidget.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QImageReader>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>

#include <ciso646>
#include <fstream>

#include "lrcinstance.h"
#include "messagewebpage.h"
#include "utils.h"
#include "webchathelpers.h"

MessageWebView::MessageWebView(QWidget *parent)
    : QWebEngineView(parent)
{
    dragDroplabel_ = new QLabel(parent);
    dragDroplabel_->hide();
    dragDroplabel_->setText("Drop files here to send");
    dragDroplabel_->setAlignment(Qt::AlignCenter);
    dragDroplabel_->installEventFilter(this);
    dragDroplabel_->setAcceptDrops(true);
    dragDroplabel_->setObjectName("dragDropLabel");

    QWebEngineProfile* profile = QWebEngineProfile::defaultProfile();
    QDir dataDir(QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation));
    dataDir.cdUp();
    auto cachePath = dataDir.absolutePath() + "/jami";
    profile->setCachePath(cachePath);
    profile->setPersistentStoragePath(cachePath);
    profile->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile->setHttpCacheType(QWebEngineProfile::NoCache);

    setPage(new MessageWebPage(profile, this));

    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    settings()->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, false);
    settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
    settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);
    settings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, false);
    settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);

    settings()->setFontSize(QWebEngineSettings::MinimumFontSize, 2);

    setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    jsBridge_ = new PrivateBridging(this);
    webChannel_ = new QWebChannel(page());
    webChannel_->registerObject(QStringLiteral("jsbridge"), jsBridge_);
    page()->setWebChannel(webChannel_);
    page()->profile()->setHttpUserAgent("jami-windows");

    connect(this, &QWebEngineView::renderProcessTerminated,
        [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            Q_UNUSED(statusCode);
            QString status;
            switch (termStatus) {
            case QWebEnginePage::NormalTerminationStatus:
                qDebug() << "MessageWebView - Render process normal exit";
                break;
            case QWebEnginePage::AbnormalTerminationStatus:
                qDebug() << "MessageWebView - Render process abnormal exit";
                break;
            case QWebEnginePage::CrashedTerminationStatus:
                qDebug() << "MessageWebView - Render process crashed";
                break;
            case QWebEnginePage::KilledTerminationStatus:
                qDebug() << "MessageWebView - Render process killed";
                break;
            }
        });
    recordWidget_ = new RecordWidget(this);
    recordWidget_->getContainer()->hide();
}

MessageWebView::~MessageWebView()
{
}

void MessageWebView::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept();
    dragDroplabel_->show();
}
void MessageWebView::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}
void MessageWebView::dragMoveEvent(QDragMoveEvent* event)
{
    event->accept();
}
void MessageWebView::dropEvent(QDropEvent* event)
{
    event->accept();
}

void MessageWebView::resizeEvent(QResizeEvent *event)
{
    event->accept();
    // resize label as messagewebview resizes
    dragDroplabel_->setGeometry(this->x(),this->y(),this->width(),this->height());
}

bool MessageWebView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != dragDroplabel_) {
        return false;
    }
    if (event->type() == QEvent::DragEnter) {
        QDragEnterEvent *dee = dynamic_cast<QDragEnterEvent *>(event);
        dee->acceptProposedAction();
        //Has to return here, otherwise it will filter out ALL events for label ! including paint events
        return true;
    } else if (event->type() == QEvent::Drop) {
        // Drop Event complete, get data
        QDropEvent *de = dynamic_cast<QDropEvent *>(event);

        const QMimeData* mimeData = de->mimeData();
        // check for our needed mime type, here a file or a list of files
        if (mimeData->hasUrls()) {
            QList<QUrl> urlList = mimeData->urls();

            // extract the local paths of the files
            for (int i = 0; i < urlList.size(); ++i) {
                // Trim file:/// from url
                QString filePath = urlList.at(i).toString().remove(0, 8);
                QByteArray imageFormat = QImageReader::imageFormat(filePath);

                //check if file is qt supported image file type
                if (!imageFormat.isEmpty()) {
                    setMessagesImageContent(filePath);
                } else {
                    setMessagesFileContent(filePath);
                }
            }
        }
        dragDroplabel_->hide();
        return true;
    } else if (event->type() == QEvent::DragLeave) {
        dragDroplabel_->hide();
        return true;
    } else {
        return false;
    }
}

void MessageWebView::setMessagesContent(const QString& text)
{
    page()->runJavaScript(QStringLiteral("replaceText('%1');").arg(text));
}

void
MessageWebView::setMessagesImageContent(const QString &path, bool isBased64)
{
    if (isBased64) {
        QString param = QString("addImage_base64('%1')").arg(path);
        page()->runJavaScript(param);
    } else {
        QString param = QString("addImage_path('%1')").arg(path);
        page()->runJavaScript(param);
    }
}

void
MessageWebView::setMessagesFileContent(const QString &path)
{
    qint64 fileSize = QFileInfo(path).size();
    QString fileName = QFileInfo(path).fileName();
    //if file name is too large, trim it
    if (fileName.length() > 15) {
        fileName = fileName.remove(12, fileName.length() - 12) + "...";
    }
    QString param = QString("addFile_path('%1','%2','%3')")
        .arg(path, fileName, Utils::humanFileSize(fileSize));
    page()->runJavaScript(param);
}

void MessageWebView::copySelectedText(QClipboard* clipboard)
{
    page()->runJavaScript(QStringLiteral("copy_text_selected();"), [clipboard, this](const QVariant& v) {
        clipboard->setText(v.toString());
    });
}

bool MessageWebView::textSelected()
{
    return textSelected_;
}

void MessageWebView::runJsText()
{
    page()->runJavaScript(QStringLiteral("isTextSelected();"), [&, this](const QVariant &val) {
        textSelected_ = val.toBool();
        emit textSelectedReady();
    });
}

void
MessageWebView::openAudioRecorder(int spikePosX, int spikePosY)
{
    if (LRCInstance::avModel().getAudioInputDevices().size() == 0) {
        // TODO: this predicate should be observed and used to show/hide the
        // send audio message button in the webview
        return;
    }
    // spikePosX, spikePosY are positions relative to Document
    auto pointOfAudioButton = mapToGlobal(QPoint(spikePosX, spikePosY));
    recordWidget_->getContainer()->move(
        pointOfAudioButton.x() - recordWidget_->width() / 2,
        pointOfAudioButton.y() - recordWidget_->height() - recordWidgetMargin_);
    recordWidget_->openRecorder(true);
}

void
MessageWebView::openVideoRecorder(int spikePosX, int spikePosY)
{
    if (LRCInstance::hasVideoCall() || LRCInstance::avModel().getDevices().size() == 0) {
        // TODO: this predicate should be observed and used to show/hide the
        // send video message button in the webview
        return;
    }
    // spikePosX, spikePosY are positions relative to Document
    auto pointOfVideoButton = mapToGlobal(QPoint(spikePosX, spikePosY));
    recordWidget_->getContainer()->move(
        pointOfVideoButton.x() - recordWidget_->width() / 2,
        pointOfVideoButton.y() - recordWidget_->height() - recordWidgetMargin_);
    recordWidget_->openRecorder(false);
}

void MessageWebView::buildView()
{
    auto html = Utils::QByteArrayFromFile(":/chatview.html");
    page()->setHtml(html, QUrl(":/chatview.html"));
    connect(this, &QWebEngineView::loadFinished, this, &MessageWebView::slotLoadFinished);
}

void
MessageWebView::slotLoadFinished()
{
    insertStyleSheet("chatcss", Utils::QByteArrayFromFile(":/chatview.css"));
    insertStyleSheet("chatwin",Utils::QByteArrayFromFile(":/chatview-windows.css"));
    page()->runJavaScript(Utils::QByteArrayFromFile(":/linkify.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/linkify-html.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/linkify-string.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/qwebchannel.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/chatview.js"), QWebEngineScript::MainWorld);
}

void
MessageWebView::insertStyleSheet(const QString &name, const QString &source)
{
    QWebEngineScript script;
    auto simplifiedCSS = source.simplified().replace("'", "\"");
    QString s = QString::fromLatin1("(function() {"\
                                    "    var node = document.createElement('style');"\
                                    "    node.id = '%1';"\
                                    "    node.innerHTML = '%2';"\
                                    "    document.head.appendChild(node);"\
                                    "})()").arg(name).arg(simplifiedCSS);
    page()->runJavaScript(s);

    script.setName(name);
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void
MessageWebView::removeStyleSheet(const QString &name)
{
    QString s = QString::fromLatin1("(function() {"\
                                    "    var element = document.getElementById('%1');"\
                                    "    element.outerHTML = '';"\
                                    "    delete element;"\
                                    "})()").arg(name);

    page()->runJavaScript(s);

    QWebEngineScript script = page()->scripts().findScript(name);
    page()->scripts().remove(script);
}

void MessageWebView::clear()
{
    QString s = QString::fromLatin1("clearMessages();");
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::setDisplayLinks(bool display)
{
    QString s = QString::fromLatin1("setDisplayLinks('%1');")
        .arg(display ? "true" : "false");
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::printNewInteraction(lrc::api::ConversationModel& conversationModel,
                                    uint64_t msgId,
                                    const lrc::api::interaction::Info& interaction)
{
    auto interactionObject = interactionToJsonInteractionObject(conversationModel, msgId, interaction).toUtf8();
    if (interactionObject.isEmpty()) {
        return;
    }
    QString s = QString::fromLatin1("addMessage(%1);")
        .arg(interactionObject.constData());
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::updateInteraction(lrc::api::ConversationModel& conversationModel,
                                  uint64_t msgId,
                                  const lrc::api::interaction::Info& interaction)
{
    auto interactionObject = interactionToJsonInteractionObject(conversationModel, msgId, interaction).toUtf8();
    if (interactionObject.isEmpty()) {
        return;
    }
    QString s = QString::fromLatin1("updateMessage(%1);")
        .arg(interactionObject.constData());
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::removeInteraction(uint64_t interactionId)
{
    QString s = QString::fromLatin1("removeInteraction(%1);")
        .arg(QString::number(interactionId));
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::printHistory(lrc::api::ConversationModel& conversationModel,
                             const std::map<uint64_t,
                             lrc::api::interaction::Info> interactions)
{
    auto interactionsStr = interactionsToJsonArrayObject(conversationModel, interactions).toUtf8();
    QString s = QString::fromLatin1("printHistory(%1);")
        .arg(interactionsStr.constData());
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::setSenderImage(const std::string& sender,
                               const std::string& senderImage)
{
    QJsonObject setSenderImageObject = QJsonObject();
    setSenderImageObject.insert("sender_contact_method", QJsonValue(QString(sender.c_str())));
    setSenderImageObject.insert("sender_image", QJsonValue(QString(senderImage.c_str())));

    auto setSenderImageObjectString = QString(QJsonDocument(setSenderImageObject).toJson(QJsonDocument::Compact));
    QString s = QString::fromLatin1("setSenderImage(%1);")
        .arg(setSenderImageObjectString.toUtf8().constData());
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::setInvitation(bool show, const std::string& contactUri, const std::string& contactId)
{
    QString s = show ? QString::fromLatin1("showInvitation(\"%1\", \"%2\")")
        .arg(QString(contactUri.c_str()))
        .arg(QString(contactId.c_str())) : QString::fromLatin1("showInvitation()");

    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

void
MessageWebView::setMessagesVisibility(bool visible)
{
    QString s = QString::fromLatin1(visible ? "showMessagesDiv();" : "hideMessagesDiv();");
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

// JS bridging incoming
Q_INVOKABLE int
PrivateBridging::log(const QString& arg)
{
    qDebug() << "JS log: " << arg;
    return 0;
}

Q_INVOKABLE int
PrivateBridging::emitMessagesCleared()
{
    if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
        emit messageView->messagesCleared();
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::emitMessagesLoaded()
{
    if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
        emit messageView->messagesLoaded();
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::deleteInteraction(const QString& arg)
{
    bool ok;
    uint64_t interactionUid = arg.toULongLong(&ok);
    if (ok) {
        LRCInstance::getCurrentConversationModel()->clearInteractionFromConversation(
            LRCInstance::getCurrentConvUid(),
            interactionUid
        );
    } else {
        qDebug() << "deleteInteraction - invalid arg" << arg;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::retryInteraction(const QString& arg)
{
    bool ok;
    uint64_t interactionUid = arg.toULongLong(&ok);
    if (ok) {
        LRCInstance::getCurrentConversationModel()->retryInteraction(
            LRCInstance::getCurrentConvUid(),
            interactionUid
        );
    } else {
        qDebug() << "retryInteraction - invalid arg" << arg;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::openFile(const QString& arg)
{
    QUrl fileUrl("file:///" + arg);
    bool ok = QDesktopServices::openUrl(fileUrl);
    if (!ok) {
        qDebug() << "Couldn't open file: " << fileUrl;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::acceptFile(const QString& arg)
{
    try {
        auto interactionUid = std::stoull(arg.toStdString());

        lrc::api::datatransfer::Info info = {};
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->getTransferInfo(interactionUid, info);

        // get full path
        std::string filename = LRCInstance::dataTransferModel().downloadDirectory.c_str();
        if (!filename.empty() && filename.back() != '/')
            filename += "/";
        auto wantedFilename = filename + info.displayName;
        auto duplicate = 0;
        while (std::ifstream(wantedFilename).good()) {
            ++duplicate;
            auto extensionIdx = info.displayName.find_last_of(".");
            if (extensionIdx == std::string::npos)
                wantedFilename = filename + info.displayName + " (" + std::to_string(duplicate) + ")";
            else
                wantedFilename = filename + info.displayName.substr(0, extensionIdx) + " (" + std::to_string(duplicate) + ")" + info.displayName.substr(extensionIdx);
        }
        LRCInstance::getCurrentConversationModel()->acceptTransfer(convUid, interactionUid, wantedFilename);
    } catch (...) {
        qDebug() << "JS bridging - exception during acceptFile: " << arg;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::refuseFile(const QString& arg)
{
    try {
        auto interactionUid = std::stoull(arg.toStdString());
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->cancelTransfer(convUid, interactionUid);
    } catch (...) {
        qDebug() << "JS bridging - exception during refuseFile:" << arg;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::sendMessage(const QString& arg)
{
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendMessage(convUid, arg.toStdString());
    } catch (...) {
        qDebug() << "JS bridging - exception during sendMessage:" << arg;
        return -1;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::sendImage(const QString& arg)
{
    if (arg.startsWith("data:image/png;base64,")) {
        //img tag contains base64 data, trim "data:image/png;base64," from data
        QByteArray data = QByteArray::fromStdString(arg.toStdString().substr(22));
        auto img_name_hash = QString::fromStdString(QCryptographicHash::hash(data, QCryptographicHash::Sha1).toHex().toStdString());
        QString fileName = "\\img_" + img_name_hash + ".png";

        QPixmap image_to_save;
        if (!image_to_save.loadFromData(QByteArray::fromBase64(data))) {
            qDebug().noquote() << "JS bridging - errors during loadFromData" << "\n";
            return -1;
        }

        QString path = QString(Utils::WinGetEnv("TEMP"))  + fileName;
        if (!image_to_save.save(path,"PNG")) {
            qDebug().noquote() << "JS bridging - errors during QPixmap save" << "\n";
            return -1;
        }

       try {
            auto convUid = LRCInstance::getCurrentConvUid();
            LRCInstance::getCurrentConversationModel()->sendFile(convUid, path.toStdString(), fileName.toStdString());
        } catch (...) {
            qDebug().noquote() << "JS bridging - exception during sendFile - base64 img" << "\n";
            return -1;
        }

    } else {
        //img tag contains file paths
        QFileInfo fi(arg);
        QString fileName = fi.fileName();
        try {
            auto convUid = LRCInstance::getCurrentConvUid();
            LRCInstance::getCurrentConversationModel()->sendFile(convUid, arg.toStdString(), fileName.toStdString());
        } catch (...) {
            qDebug().noquote() << "JS bridging - exception during sendFile - image from path" << "\n";
            return -1;
        }
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::sendFile(const QString&path)
{
    qDebug() << "JS bridging - MessageWebView::sendFile";
    QFileInfo fi(path);
    QString fileName = fi.fileName();
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendFile(convUid, path.toStdString(), fileName.toStdString());
    } catch (...) {
        qDebug() << "JS bridging - exception during sendFile";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::selectFile()
{
    QString filePath = QFileDialog::getOpenFileName((QWidget*)this->parent(), tr("Choose File"), "", tr("Files") + " (*)");
    if (filePath.length() == 0)
        return 0;
    QByteArray imageFormat = QImageReader::imageFormat(filePath);

    if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
        if (!imageFormat.isEmpty()) {
            messageView->setMessagesImageContent(filePath);
            return 0;
        }
        messageView->setMessagesFileContent(filePath);
        return 0;
    }
    return -1;
}

Q_INVOKABLE int
PrivateBridging::acceptInvitation()
{
    try {
        emit qobject_cast<MessageWebView*>(this->parent())->invitationAccepted();
    } catch (...) {
        qDebug() << "JS bridging - exception during acceptInvitation";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::refuseInvitation()
{
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->removeConversation(convUid, false);
        if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
            messageView->setInvitation(false);
            emit messageView->conversationRemoved();
        }
    } catch (...) {
        qDebug() << "JS bridging - exception during refuseInvitation";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::blockConversation()
{
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->removeConversation(convUid, true);
        if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
            messageView->setInvitation(false);
            emit messageView->conversationRemoved();
        }
    } catch (...) {
        qDebug() << "JS bridging - exception during blockConversation";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::emitPasteKeyDetected()
{
    if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
        emit messageView->pasteKeyDetected();
    } else {
        qDebug() << "JS bridging - exception during emitPasteKeyDetected";
    }
    return 0;
}

int
PrivateBridging::openAudioRecorder(int spikePosX, int spikePosY)
{
    //call the open audio recorder function in messageweview
    try {
        if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
            messageView->openAudioRecorder(spikePosX, spikePosY);
        }
    } catch (...) {
        qDebug() << "JS bridging - exception during openAudioRecorder!";
    }
    return 0;
}

int
PrivateBridging::openVideoRecorder(int spikePosX, int spikePosY)
{
    //call the open video recorder function in messageweview
    try {
        if (auto messageView = qobject_cast<MessageWebView*>(this->parent())) {
            messageView->openVideoRecorder(spikePosX, spikePosY);
        }
    } catch (...) {
        qDebug() << "JS bridging - exception during openVideoRecorder!";
    }
    return 0;
}
