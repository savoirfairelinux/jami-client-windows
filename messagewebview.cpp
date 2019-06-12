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

#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QMessagebox>

#include <ciso646>
#include <fstream>

#include "lrcinstance.h"
#include "messagewebpage.h"
#include "utils.h"
#include "webchathelpers.h"

MessageWebView::MessageWebView(QWidget *parent)
    : QWebEngineView(parent)
{
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
}

MessageWebView::~MessageWebView()
{
}

void MessageWebView::setMessagesContent(const QString& text)
{
    page()->runJavaScript(QStringLiteral("var ele = document.getElementById('message_test'); ele.innerHTML += '%1'; ").arg(text));
}

void
MessageWebView::setMessagesImageContent(const QString &text, const short& type)
{
    if (type == 0) {
        QString param = QString("addImage_base64('%1')").arg(text);
        page()->runJavaScript(param);
    } else if (type == 1) {
        QString param = QString("addImage_path('%1')").arg(text);
        page()->runJavaScript(param);
    }
}

void MessageWebView::copySelectedText(QClipboard* clipboard)
{
    page()->runJavaScript(QStringLiteral("copy_text_selected();"), [clipboard, this](const QVariant& v) {
        clipboard->setText(v.toString());
    });
}

void MessageWebView::buildView()
{
    auto html = Utils::QByteArrayFromFile(":/web/chatview.html");
    page()->setHtml(html, QUrl(":/web/chatview.html"));
    connect(this, &QWebEngineView::loadFinished, this, &MessageWebView::slotLoadFinished);
}

void
MessageWebView::slotLoadFinished()
{
    insertStyleSheet("chatcss", Utils::QByteArrayFromFile(":/web/chatview.css"));
    page()->runJavaScript(Utils::QByteArrayFromFile(":/web/linkify.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/web/linkify-html.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/web/linkify-string.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/web/qwebchannel.js"), QWebEngineScript::MainWorld);
    page()->runJavaScript(Utils::QByteArrayFromFile(":/web/chatview.js"), QWebEngineScript::MainWorld);
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
            LRCInstance::getSelectedConvUid(),
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
            LRCInstance::getSelectedConvUid(),
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
    QDesktopServices::openUrl(arg);
    return 0;
}

Q_INVOKABLE int
PrivateBridging::acceptFile(const QString& arg)
{
    try {
        auto interactionUid = std::stoull(arg.toStdString());

        lrc::api::datatransfer::Info info = {};
        auto convUid = LRCInstance::getSelectedConvUid();
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
        auto convUid = LRCInstance::getSelectedConvUid();
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
        auto convUid = LRCInstance::getSelectedConvUid();
        LRCInstance::getCurrentConversationModel()->sendMessage(convUid, arg.toStdString());
    } catch (...) {
        qDebug() << "JS bridging - exception during sendMessage:" << arg;
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::sendImage(const QString& arg)
{
    if (arg.startsWith("data:image/png;base64,")) {
        //img tag contains base64 data, trim "data:image/png;base64," from data
        QByteArray data = QByteArray::fromStdString(arg.toStdString().substr(22));
        qDebug().noquote() << "\n" << QString::fromStdString(arg.toStdString().substr(22));
        QPixmap image_to_save;
        if (!image_to_save.loadFromData(QByteArray::fromBase64(data))) {
            QMessageBox::information(0, "", "dsdadsasd");
        }

        QString path = QString(Utils::WinGetEnv("TEMP"))  + "\\img.png";
        if (!image_to_save.save(path,"PNG")) {
            QMessageBox::information(0, "", path);
            qDebug() << "JS bridging - errors during sendImage(data)";
            return -1;
        }
        QFileInfo fi(path);
        QString fileName = fi.fileName();
       try {
            auto convUid = LRCInstance::getSelectedConvUid();
            LRCInstance::getCurrentConversationModel()->sendFile(convUid, path.toStdString(), fileName.toStdString());
        } catch (...) {
            qDebug() << "JS bridging - exception during sendImage(data)";
        }
    } else {
        //img tag contains file paths
        QFileInfo fi(arg);
        QString fileName = fi.fileName();
        try {
            auto convUid = LRCInstance::getSelectedConvUid();
            LRCInstance::getCurrentConversationModel()->sendFile(convUid, arg.toStdString(), fileName.toStdString());
        } catch (...) {
            qDebug() << "JS bridging - exception during sendImage(file)";
        }
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::sendFile()
{
    qDebug() << "JS bridging - MessageWebView::sendFile";
    QString filePath = QFileDialog::getOpenFileName((QWidget*)this->parent(), tr("Choose File"), "", tr("Files (*)"));
    QFileInfo fi(filePath);
    QString fileName = fi.fileName();
    try {
        auto convUid = LRCInstance::getSelectedConvUid();
        LRCInstance::getCurrentConversationModel()->sendFile(convUid, filePath.toStdString(), fileName.toStdString());
    } catch (...) {
        qDebug() << "JS bridging - exception during sendFile";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::acceptInvitation()
{
    try {
        auto convUid = LRCInstance::getSelectedConvUid();
        LRCInstance::getCurrentConversationModel()->makePermanent(convUid);
        qobject_cast<MessageWebView*>(this->parent())->setInvitation(false);
    } catch (...) {
        qDebug() << "JS bridging - exception during acceptInvitation";
    }
    return 0;
}

Q_INVOKABLE int
PrivateBridging::refuseInvitation()
{
    try {
        auto convUid = LRCInstance::getSelectedConvUid();
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
        auto convUid = LRCInstance::getSelectedConvUid();
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
