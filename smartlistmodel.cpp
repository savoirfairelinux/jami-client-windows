/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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

#include "smartlistmodel.h"

// Qt
#include <QDateTime>

// LRC
#include "globalinstances.h"
#include "api/contactmodel.h"
#include "api/conversationmodel.h"

// Client
#include "pixbufmanipulator.h"
#include "messagemodel.h"
#include "utils.h"

SmartListModel::SmartListModel(const lrc::api::account::Info &acc, QObject *parent)
    : QAbstractItemModel(parent),
    acc_(acc)
{
}

int SmartListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return acc_.conversationModel->allFilteredConversations().size();
    }
    return 0; // A valid QModelIndex returns 0 as no entry has sub-elements
}

int SmartListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant SmartListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    const auto& item = acc_.conversationModel->filteredConversation(index.row());
    if (item.participants.size() > 0) {
        try {
            switch (role) {
            case Role::Picture:
            case Qt::DecorationRole:
                return GlobalInstances::pixmapManipulator().decorationRole(item, acc_);
            case Role::DisplayName:
            case Qt::DisplayRole:
            {
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(Utils::bestNameForContact(contact)));
            }
            case Role::DisplayID:
            {
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(Utils::bestIdForContact(contact)));
            }
            case Role::Presence:
            {
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(contact.isPresent);
            }
            case Role::URI:
            {
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(contact.profileInfo.uri));
            }
            case Role::UnreadMessagesCount:
                return QVariant(item.unreadMessages);
                case Role::LastInteractionDate:
                {
                    auto& date = item.interactions.at(item.lastMessageUid).timestamp;
                    return QVariant(QString::fromStdString(Utils::formatTimeString(date)));
                }
            case Role::LastInteraction:
                return QVariant(QString::fromStdString(item.interactions.at(item.lastMessageUid).body));
            }
        } catch (...) {}
    }
    return QVariant();
}

QModelIndex SmartListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (column != 0) {
        return QModelIndex();
    }

    if (row >= 0 && row < rowCount()) {
        return createIndex(row, column);
    }
    return QModelIndex();
}

QModelIndex SmartListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

Qt::ItemFlags SmartListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    } else {
        return QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;
    }
}

void SmartListModel::setFilter(const QString &filter)
{
    emit layoutAboutToBeChanged();
    if (!filter.isEmpty()) {
        acc_.conversationModel->setFilter(filter.toStdString());
    } else {
        acc_.conversationModel->setFilter(acc_.profileInfo.type);
    }
    emit layoutChanged();
}
