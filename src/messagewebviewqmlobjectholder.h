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
class MessageWebViewQmlObjectHolder : public QObject {
    Q_OBJECT

public:
    explicit MessageWebViewQmlObjectHolder(QObject* parent = 0);
    ~MessageWebViewQmlObjectHolder();

    // Must call Q_INVOKABLE so that this function can be used in QML
    Q_INVOKABLE void setMessageWebViewQmlObject(QObject* obj);
    Q_INVOKABLE void setupChatView(const QString& uid);

    // run corrsponding js functions
    void setMessagesVisibility(bool visible);
    void requestSendMessageContent();
    void setInvitation(bool show, const QString& contactUri = "", const QString& contactId = "");
    void clear();
    void printHistory(lrc::api::ConversationModel& conversationModel,
                      const std::map<uint64_t, lrc::api::interaction::Info> interactions);
    void setSenderImage(const QString& sender, const QString& senderImage);

public slots:
    void slotSendMessageContentSaved(const QString& content);
    void slotMessagesCleared();
    void slotMessagesLoaded();

private:
    void setConversationProfileData(const lrc::api::conversation::Info& convInfo);

    // Object pointer
    QObject* messageWebViewQmlObject_;
    QString LastConvUid_;
    QString currentConvUid_;
};