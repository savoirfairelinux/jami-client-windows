/***************************************************************************
 * Copyright (C) 2017-2018 by Savoir-faire Linux                           *
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

#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QWebEnginePage>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineSettings>
#include <QWebChannel>

#include <ciso646>

#include "utils.h"
#include "messagemodel.h"
#include "webchathelpers.h"
#include "messagewebpage.h"

MessageWebView::MessageWebView(QWidget *parent)
    : QWebEngineView(parent)
{
    setPage(new MessageWebPage());
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

    jsBridge_ = new PrivateBridging(this);
    webChannel_ = new QWebChannel(page());
    webChannel_->registerObject(QStringLiteral("jsbridge"), jsBridge_);
    page()->setWebChannel(webChannel_);

    connect(this, &QWebEngineView::renderProcessTerminated,
        [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
            QString status;
            switch (termStatus) {
            case QWebEnginePage::NormalTerminationStatus:
                qDebug() << "Render process normal exit";
                break;
            case QWebEnginePage::AbnormalTerminationStatus:
                qDebug() << "Render process abnormal exit";
                break;
            case QWebEnginePage::CrashedTerminationStatus:
                qDebug() << "Render process crashed";
                break;
            case QWebEnginePage::KilledTerminationStatus:
                qDebug() << "Render process killed";
                break;
            }
        });
}

MessageWebView::~MessageWebView()
{
}

void MessageWebView::buildView()
{
    //QUrl chatSourceHTML("qrc:/webchat/chatview.html");
    QString path("C:/Users/fner/Projects/ring-project/client-windows/web/");

    /*QUrl chatSourceHTML("file:///" + path + "chatview.html");
    page()->load(chatSourceHTML);*/

    auto html = Utils::QByteArrayFromFile(path + "chatview.html");
    page()->setHtml(html, QUrl("qrc:/web/chatview.html"));

    connect(this, &QWebEngineView::loadFinished,
        [this, path] {
            //QString cssFileName(":/web/chatview.css");
            insertStyleSheet("chatcss", Utils::QByteArrayFromFile(path + "chatview.css"));
            page()->runJavaScript(Utils::QByteArrayFromFile(path + "chatview.js"), QWebEngineScript::MainWorld);
            /*insertJavascriptLibrary(Utils::QByteArrayFromFile(path + "linkify.js"));
            insertJavascriptLibrary(Utils::QByteArrayFromFile(path + "linkify-html.js"));
            insertJavascriptLibrary(Utils::QByteArrayFromFile(path + "linkify-string.js"));*/
        });
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

void
MessageWebView::insertJavascriptLibrary(const QString & source)
{
    QWebEngineScript script;
    auto simplifiedJS = source.simplified().replace("'", "\"");
    QString s = QString::fromLatin1("(function() {"\
                                    "    var node = document.createElement('script');"\
                                    "    node.innerHTML = '%1';"\
                                    "    document.body.appendChild(node);"\
                                    "})()").arg(simplifiedJS);
    page()->runJavaScript(s, QWebEngineScript::MainWorld);

    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
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
MessageWebView::setInvitation(bool show, const std::string& contactUri)
{
    QString s = QString::fromLatin1(show ? "showInvitation(\"%1\")" : "showInvitation()")
        .arg(QString(contactUri.c_str()));
    page()->runJavaScript(s, QWebEngineScript::MainWorld);
}

// JS bridging incoming

Q_INVOKABLE int
PrivateBridging::deleteInteraction(const QString& arg)
{

    qDebug() << "JS bridging - MessageWebView::deleteInteraction:" << arg;
    return 0;
}

Q_INVOKABLE int
PrivateBridging::retryInteraction(const QString& arg)
{
    qDebug() << "JS bridging - MessageWebView::retryInteraction:" << arg;
    return 0;
}

Q_INVOKABLE int
PrivateBridging::openFile(const QString& arg)
{
    qDebug() << "JS bridging - MessageWebView::openFile:" << arg;
    return 0;
}

Q_INVOKABLE int
PrivateBridging::acceptFile(const QString& arg)
{
    qDebug() << "JS bridging - MessageWebView::acceptFile:" << arg;
    return 0;
}

Q_INVOKABLE int
PrivateBridging::refuseFile(const QString& arg)
{
    qDebug() << "JS bridging - MessageWebView::refuseFile:" << arg;
    return 0;
}