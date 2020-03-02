/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "messagewebviewqmlobjectholder.h"
#include "webchathelpers.h"

#include "utils.h"

MessageWebViewQmlObjectHolder::MessageWebViewQmlObjectHolder(QObject* parent)
    : QObject(parent)
{
}

MessageWebViewQmlObjectHolder::~MessageWebViewQmlObjectHolder()
{
}

void
MessageWebViewQmlObjectHolder::setMessageWebViewQmlObject(QObject* obj)
{
    // Set the object pointer
    messageWebViewQmlObject_ = obj;

    auto currentConversationModel = LRCInstance::getCurrentAccountInfo().conversationModel.get();

    connect(messageWebViewQmlObject_, SIGNAL(sendMessage(QString)), this, SLOT(slotSendMessage(const QString&)));

    newInteractionConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newInteraction,
        [this](const QString& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
                auto accountId = LRCInstance::getCurrAccId();
                newInteraction(accountId, convUid, interactionId, interaction);
        }
    );
}

void
MessageWebViewQmlObjectHolder::setupChatView(const QString& uid)
{
    if (!LRCInstance::getCurrentConvUid().isEmpty()) {
        LastConvUid_ = LRCInstance::getCurrentConvUid();
    }
    LRCInstance::setSelectedConvId(uid);

    auto& convInfo = LRCInstance::getConversationFromConvUid(uid);
    if (convInfo.uid.isEmpty()) {
        return;
    }

    QString contactURI = convInfo.participants.at(0);

    bool isContact = false;
    auto selectedAccountId = LRCInstance::getCurrAccId();
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(selectedAccountId);
    bool isRINGAccount = accountInfo.profileInfo.type == lrc::api::profile::Type::RING;

    lrc::api::profile::Type contactType;
    try {
        auto contactInfo = accountInfo.contactModel->getContact(contactURI);
        if (contactInfo.isTrusted) {
            isContact = true;
        }
        contactType = contactInfo.profileInfo.type;
    }
    catch (...) {}

    bool shouldShowSendContactRequestBtn = !isContact && contactType != lrc::api::profile::Type::SIP;

    QMetaObject::invokeMethod(messageWebViewQmlObject_,
                              "setSendContactRequestButtonVisible",
                              Q_ARG(QVariant, shouldShowSendContactRequestBtn));

    setMessagesVisibility(false);
    //connect(LRCInstance::getCurrentConversationModel(), &ConversationModel::composingStatusChanged, ui->messageView, &MessageWebView::contactIsComposing);

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(sendMessageContentSaved(QString)), this, SLOT(slotSendMessageContentSaved(QString)));

    requestSendMessageContent();
}

void
MessageWebViewQmlObjectHolder::slotSendMessageContentSaved(const QString& content)
{
    if (!LastConvUid_.isEmpty()) {
        //LRCInstance::setContentDraft(LastConvUid_, LRCInstance::getCurrAccId(), content);
    }

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(messagesCleared()), this, SLOT(slotMessagesCleared()));

    setInvitation(false);
    clear();
    //auto restoredContent = LRCInstance::getContentDraft(convInfo.uid, accountId);
    //ui->messageView->setSendMessageContent(restoredContent);
    //ui->smartList->update();
}

void
MessageWebViewQmlObjectHolder::slotMessagesCleared()
{
    auto& convInfo = LRCInstance::getConversationFromConvUid(LRCInstance::getCurrentConvUid());
    auto convModel = LRCInstance::getCurrentConversationModel();

    printHistory(*convModel, convInfo.interactions);

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(messagesLoaded()), this, SLOT(slotMessagesLoaded()));

    setConversationProfileData(convInfo);
}

void
MessageWebViewQmlObjectHolder::slotMessagesLoaded()
{
    setMessagesVisibility(true);
}

void
MessageWebViewQmlObjectHolder::slotSendMessage(const QString& message)
{
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendMessage(convUid, message);
    }
    catch (...) {
        qDebug() << "Exception during sendMessage:" << message;
    }
}

void
MessageWebViewQmlObjectHolder::setConversationProfileData(const lrc::api::conversation::Info& convInfo)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto accInfo = &LRCInstance::getCurrentAccountInfo();
    auto contactUri = convInfo.participants.front();

    if (contactUri.isEmpty()) {
        return;
    }
    try {
        auto& contact = accInfo->contactModel->getContact(contactUri);
        auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
        setInvitation(contact.profileInfo.type == lrc::api::profile::Type::PENDING, bestName, contactUri);

        if (!contact.profileInfo.avatar.isEmpty()) {
            setSenderImage(contactUri, contact.profileInfo.avatar);
        }
        else {
            auto avatar = Utils::conversationPhoto(convInfo.uid, *accInfo);
            QByteArray ba;
            QBuffer bu(&ba);
            avatar.save(&bu, "PNG");
            setSenderImage(contactUri, QString::fromLocal8Bit(ba.toBase64()));
        }
    }
    catch (...) {}
}

void
MessageWebViewQmlObjectHolder::newInteraction(const QString& accountId, const QString& convUid,
                                              uint64_t interactionId, const interaction::Info& interaction)
{
    Q_UNUSED(interactionId);
    try {
        auto& accountInfo = LRCInstance::getAccountInfo(accountId);
        auto& convModel = accountInfo.conversationModel;
        auto& conversation = LRCInstance::getConversationFromConvUid(convUid, accountId);
        if (conversation.uid.isEmpty()) {
            return;
        }
        if (!interaction.authorUri.isEmpty() &&
           (!QGuiApplication::focusWindow() || LRCInstance::getCurrAccId() != accountId)) {
            auto bestName = Utils::bestNameForConversation(conversation, *convModel);
            //Utils::showSystemNotification(this, bestName, interaction.body);
        }
        //updateConversationsFilterWidget();
        if (convUid != LRCInstance::getCurrentConvUid()) {
            return;
        }
        convModel->clearUnreadInteractions(convUid);
        printNewInteraction(*convModel, interactionId, interaction);
        if (interaction.type != interaction::Type::CALL) {
            //ui->videoView->simulateShowChatview(true);
        }
    }
    catch (...) {}
}

// Js invoke
void
MessageWebViewQmlObjectHolder::setMessagesVisibility(bool visible)
{
    QString s = QString::fromLatin1(visible ? "showMessagesDiv();" : "hideMessagesDiv();");
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::requestSendMessageContent()
{
    QString s = QString::fromLatin1("requestSendMessageContent();");
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::setInvitation(bool show, const QString& contactUri, const QString& contactId)
{
    QString s = show ? QString::fromLatin1("showInvitation(\"%1\", \"%2\")")
                .arg(contactUri)
                .arg(contactId) : QString::fromLatin1("showInvitation()");

    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::clear()
{
    QString s = QString::fromLatin1("clearMessages();");
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::printHistory(lrc::api::ConversationModel& conversationModel,
                                            const std::map<uint64_t, lrc::api::interaction::Info> interactions)
{
    auto interactionsStr = interactionsToJsonArrayObject(conversationModel, interactions).toUtf8();
    QString s = QString::fromLatin1("printHistory(%1);") .arg(interactionsStr.constData());
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::setSenderImage(const QString& sender, const QString& senderImage)
{
    QJsonObject setSenderImageObject = QJsonObject();
    setSenderImageObject.insert("sender_contact_method", QJsonValue(sender));
    setSenderImageObject.insert("sender_image", QJsonValue(senderImage));

    auto setSenderImageObjectString = QString(QJsonDocument(setSenderImageObject).toJson(QJsonDocument::Compact));
    QString s = QString::fromLatin1("setSenderImage(%1);") .arg(setSenderImageObjectString.toUtf8().constData());
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}

void
MessageWebViewQmlObjectHolder::printNewInteraction(lrc::api::ConversationModel& conversationModel,
                                                   uint64_t msgId,
                                                   const lrc::api::interaction::Info& interaction)
{
    auto interactionObject = interactionToJsonInteractionObject(conversationModel, msgId, interaction).toUtf8();
    if (interactionObject.isEmpty()) {
        return;
    }
    QString s = QString::fromLatin1("addMessage(%1);").arg(interactionObject.constData());
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "webViewRunJavaScript", Q_ARG(QVariant, s));
}