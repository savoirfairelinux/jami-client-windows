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
}

void
MessageWebViewQmlObjectHolder::setupChatView(const QString& uid)
{
    if (!currentConvUid_.isEmpty()) {
        LastConvUid_ = currentConvUid_;
    }
    currentConvUid_ = uid;

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

    QMetaObject::invokeMethod(messageWebViewQmlObject_, "setSendContactRequestButtonVisible", Q_ARG(QVariant, shouldShowSendContactRequestBtn));
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "setMessagesVisibility", Q_ARG(QVariant, false));

    //connect(LRCInstance::getCurrentConversationModel(), &ConversationModel::composingStatusChanged, ui->messageView, &MessageWebView::contactIsComposing);

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(toSendMessageContentSaved(QString)), this, SLOT(slotSendMessageContentSaved(QString)));

    QMetaObject::invokeMethod(messageWebViewQmlObject_, "requestSendMessageContent");
}

void
MessageWebViewQmlObjectHolder::slotSendMessageContentSaved(const QString& content)
{
    if (!LastConvUid_.isEmpty()) {
        //LRCInstance::setContentDraft(LastConvUid_, LRCInstance::getCurrAccId(), content);
    }

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(toMessagesCleared()), this, SLOT(slotMessagesCleared()));

    QMetaObject::invokeMethod(messageWebViewQmlObject_, "setInvitation", Q_ARG(QVariant, false), Q_ARG(QVariant, ""), Q_ARG(QVariant, ""));
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "clear");
    //auto restoredContent = LRCInstance::getContentDraft(convInfo.uid, accountId);
    //ui->messageView->setSendMessageContent(restoredContent);
    //ui->smartList->update();
}

void
MessageWebViewQmlObjectHolder::slotMessagesCleared()
{
    auto& convInfo = LRCInstance::getConversationFromConvUid(currentConvUid_);
    auto convModel = LRCInstance::getCurrentConversationModel();

    auto interactionsStr = interactionsToJsonArrayObject(*convModel, convInfo.interactions).toUtf8();
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "printHistory", Q_ARG(QVariant, interactionsStr.constData()));

    Utils::oneShotConnect(messageWebViewQmlObject_, SIGNAL(toMessagesLoaded()), this, SLOT(slotMessagesLoaded()));

    setConversationProfileData(convInfo);
}

void
MessageWebViewQmlObjectHolder::slotMessagesLoaded()
{
    QMetaObject::invokeMethod(messageWebViewQmlObject_, "setMessagesVisibility", Q_ARG(QVariant, true));
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
        QMetaObject::invokeMethod(messageWebViewQmlObject_, "setInvitation",
            Q_ARG(QVariant, contact.profileInfo.type == lrc::api::profile::Type::PENDING),
            Q_ARG(QVariant, bestName),
            Q_ARG(QVariant, contactUri)
        );
        if (!contact.profileInfo.avatar.isEmpty()) {
            QMetaObject::invokeMethod(messageWebViewQmlObject_, "setSenderImage",
                Q_ARG(QVariant, getSenderImageObjectString(contactUri, contact.profileInfo.avatar).toUtf8().constData())
            );
        }
        else {
            auto avatar = Utils::conversationPhoto(convInfo.uid, *accInfo);
            QByteArray ba;
            QBuffer bu(&ba);
            avatar.save(&bu, "PNG");
            QMetaObject::invokeMethod(messageWebViewQmlObject_, "setSenderImage",
                Q_ARG(QVariant, getSenderImageObjectString(contactUri, QString::fromLocal8Bit(ba.toBase64())).toUtf8().constData())
            );
        }
    }
    catch (...) {}
}

QString
MessageWebViewQmlObjectHolder::getSenderImageObjectString(const QString& sender, const QString& senderImage)
{
    QJsonObject setSenderImageObject = QJsonObject();
    setSenderImageObject.insert("sender_contact_method", QJsonValue(sender));
    setSenderImageObject.insert("sender_image", QJsonValue(senderImage));

    return QString(QJsonDocument(setSenderImageObject).toJson(QJsonDocument::Compact));
}