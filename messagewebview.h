/***************************************************************************
 * Copyright (C) 2018 by Savoir-faire Linux                                *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include <QDebug>
#include <QWebEngineView>

#include "api/conversationmodel.h"

class PrivateBridging : public QObject
{
    Q_OBJECT
public:
    explicit PrivateBridging(QObject* parent = nullptr) : QObject(parent) {};
    ~PrivateBridging() {};

    // exposed to JS through QWebChannel
    Q_INVOKABLE int deleteInteraction(const QString& arg);
    Q_INVOKABLE int retryInteraction(const QString& arg);
    Q_INVOKABLE int openFile(const QString& arg);
    Q_INVOKABLE int acceptFile(const QString& arg);
    Q_INVOKABLE int refuseFile(const QString& arg);
    Q_INVOKABLE int sendMessage(const QString & arg);
    Q_INVOKABLE int sendFile();
    Q_INVOKABLE int log(const QString& arg);
    Q_INVOKABLE int acceptInvitation();
    Q_INVOKABLE int discardInvitation();
    Q_INVOKABLE int blockConversation();
};

class MessageWebView : public QWebEngineView
{
    Q_OBJECT
public:
    explicit MessageWebView(QWidget* parent = nullptr);
    ~MessageWebView();

    void buildView();

    void insertStyleSheet(const QString &name, const QString &source);
    void removeStyleSheet(const QString &name);

    void clear();
    void setDisplayLinks(bool display);
    void printNewInteraction(lrc::api::ConversationModel& conversationModel,
                             uint64_t msgId,
                             const lrc::api::interaction::Info& interaction);
    void updateInteraction(lrc::api::ConversationModel& conversationModel,
                           uint64_t msgId,
                           const lrc::api::interaction::Info& interaction);
    void removeInteraction(uint64_t interactionId);
    void printHistory(lrc::api::ConversationModel& conversationModel,
                      const std::map<uint64_t,
                      lrc::api::interaction::Info> interactions);
    void setSenderImage(const std::string& sender,
                        const std::string& senderImage);
    void setInvitation(bool show, const std::string& contactUri, const std::string& contactId);
    void hideMessages();

private slots:
    void slotLoadFinished();

private:
    QWebChannel* webChannel_;
    PrivateBridging* jsBridge_;

};
