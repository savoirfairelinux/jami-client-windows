/***************************************************************************
* Copyright (C) 2017 by Savoir-faire Linux                                 *
* Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>           *
*                                                                          *
* This program is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 3 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
***************************************************************************/
#pragma once

// Qt include
#include <QAbstractItemModel>

// LRC
#include "api/account.h"
#include "api/conversation.h"
#include "api/contact.h"

 // message sequencing
typedef enum class MsgSeq {
    SINGLE_WITH_TIME = 0,
    SINGLE_WITHOUT_TIME = 1,
    FIRST_WITH_TIME = 2,
    FIRST_WITHOUT_TIME = 3,
    MIDDLE_IN_SEQUENCE = 5,
    LAST_IN_SEQUENCE = 6,
};

class MessageModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    using AccountInfo = lrc::api::account::Info;
    using ConversationInfo = lrc::api::conversation::Info;
    using InteractionInfo = lrc::api::interaction::Info;

    enum Role {
        Body = Qt::UserRole + 1,
        DisplayName,
        Picture,
        Presence,
        Status,
        DataTransferStatus,
        InteractionDate,
        Direction,
        Type,
        Conversation
    };

    explicit MessageModel(const ConversationInfo& conv, const AccountInfo& acc, QObject *parent = 0);

    // QAbstractItemModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    MsgSeq computeSequencing(const QModelIndex& index);
    bool sequenceChanged(InteractionInfo & firstInteraction, InteractionInfo & secondInteraction);
    bool sequenceTimeChanged(InteractionInfo& firstInteraction, InteractionInfo& secondInteraction);
    bool sequenceAuthorChanged(InteractionInfo & firstInteraction, InteractionInfo & secondInteraction);
    const std::string & timeForMessage(time_t msgTime);

private:
    ConversationInfo conv_;
    const AccountInfo& acc_;

    std::map<int, MsgSeq> sequencingCache_;
};
