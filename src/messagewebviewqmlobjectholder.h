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

#pragma once

#include "lrcinstance.h"

#include <QObject>
#include <QString>

// to ease the logic with the power of c++
class MessageWebViewQmlObjectHolder : public QObject
{
    Q_OBJECT

public:
    explicit MessageWebViewQmlObjectHolder(QObject *parent = 0);
    ~MessageWebViewQmlObjectHolder();

    // Must call Q_INVOKABLE so that this function can be used in QML, qml to c++
    Q_INVOKABLE void setMessageWebViewQmlObject(QObject *obj);
    Q_INVOKABLE void setupChatView(const QString &uid);
    Q_INVOKABLE void connectConversationModel();
    Q_INVOKABLE void sendContactRequest();
    Q_INVOKABLE void updateConversationForAddedContact();

    // run corrsponding js functions, c++ to qml
    void setMessagesVisibility(bool visible);
    void requestSendMessageContent();
    void setInvitation(bool show, const QString &contactUri = "", const QString &contactId = "");
    void clear();
    void printHistory(lrc::api::ConversationModel &conversationModel,
                      const std::map<uint64_t, lrc::api::interaction::Info> interactions);
    void setSenderImage(const QString &sender, const QString &senderImage);
    void printNewInteraction(lrc::api::ConversationModel &conversationModel,
                             uint64_t msgId,
                             const lrc::api::interaction::Info &interaction);
    void updateInteraction(lrc::api::ConversationModel &conversationModel,
                           uint64_t msgId,
                           const lrc::api::interaction::Info &interaction);
    void setMessagesImageContent(const QString &path, bool isBased64 = false);
    void setMessagesFileContent(const QString &path);
    void removeInteraction(uint64_t interactionId);

public slots:
    void slotSendMessageContentSaved(const QString &content);
    void slotMessagesCleared();
    void slotMessagesLoaded();
    void slotSendMessage(const QString &message);
    void slotSendImage(const QString &message);
    void slotSendFile(const QString &message);
    void slotSetNewMessagesContent(const QString &path);
    void slotDeleteInteraction(const QString &arg);

private:
    void setConversationProfileData(const lrc::api::conversation::Info &convInfo);
    void newInteraction(const QString &accountId,
                        const QString &convUid,
                        uint64_t interactionId,
                        const interaction::Info &interaction);

    // Object pointer
    QObject *messageWebViewQmlObject_;

    QString LastConvUid_;

    // interaction connections
    QMetaObject::Connection newInteractionConnection_;
    QMetaObject::Connection interactionStatusUpdatedConnection_;
    QMetaObject::Connection interactionRemovedConnection_;
};