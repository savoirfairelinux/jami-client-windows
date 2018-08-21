/***************************************************************************
* Copyright (C) 2017 by Savoir-faire Linux                                *
* Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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

// Qt include
#include <QAbstractListModel>
// LRC
#include "api/account.h"
#include "api/conversation.h"
#include "api/contact.h"

namespace lrc { namespace api { class ConversationModel; } }

class SmartListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using AccountInfo = lrc::api::account::Info;
    using ConversationInfo = lrc::api::conversation::Info;
    using ContactInfo = lrc::api::contact::Info;

    enum Role {
        DisplayName = Qt::UserRole + 1,
        DisplayID,
        Picture,
        Presence,
        URI,
        UnreadMessagesCount,
        LastInteractionDate,
        LastInteraction
    };

    explicit SmartListModel(const AccountInfo& acc, QObject *parent = 0);

    // QAbstractItemModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    // ConversationModel
    void setFilter(const QString& filter);

public slots:
    void selectConversation(const QModelIndex& current, const QModelIndex &previous);

private:
    const AccountInfo& acc_;
};
