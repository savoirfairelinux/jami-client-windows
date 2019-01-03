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
#include "utils.h"
#include "lrcinstance.h"

SmartListModel::SmartListModel(const std::string& accId, QObject *parent)
    : QAbstractItemModel(parent),
    accId_(accId)
{
}

int SmartListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        auto& accInfo = LRCInstance::accountModel().getAccountInfo(accId_);
        return accInfo.conversationModel->allFilteredConversations().size();
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

    auto& accInfo = LRCInstance::accountModel().getAccountInfo(accId_);
    const auto& item = accInfo.conversationModel->filteredConversation(index.row());
    if (item.participants.size() > 0) {
        try {
            switch (role) {
            case Role::Picture:
            case Qt::DecorationRole:
                return GlobalInstances::pixmapManipulator().decorationRole(item, accInfo);
            case Role::DisplayName:
            case Qt::DisplayRole:
            {
                auto& contact = accInfo.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(Utils::bestNameForContact(contact)));
            }
            case Role::DisplayID:
            {
                auto& contact = accInfo.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(Utils::bestIdForContact(contact)));
            }
            case Role::Presence:
            {
                auto& contact = accInfo.contactModel->getContact(item.participants[0]);
                return QVariant(contact.isPresent);
            }
            case Role::URI:
            {
                auto& contact = accInfo.contactModel->getContact(item.participants[0]);
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
            case Role::LastInteractionType:
                return QVariant(Utils::toUnderlyingValue(item.interactions.at(item.lastMessageUid).type));
            case Role::ContactType:
            {
                auto& contact = accInfo.contactModel->getContact(item.participants[0]);
                return QVariant(Utils::toUnderlyingValue(contact.profileInfo.type));
            }
            case Role::UID:
                return QVariant(QString::fromStdString(item.uid));
            case Role::ContextMenuOpen:
                return QVariant(isContextMenuOpen);
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
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    auto type = Utils::toEnum<lrc::api::profile::Type>(data(index, Role::ContactType).value<int>());
    auto displayName = data(index, Role::DisplayName).value<QString>();
    auto uid = data(index, Role::UID).value<QString>();
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    } else if ( type == lrc::api::profile::Type::TEMPORARY &&
                uid.isEmpty()) {
        flags &= ~(Qt::ItemIsSelectable);
    }
    return flags;
}

void
SmartListModel::setAccount(const std::string& accId)
{
    beginResetModel();
    accId_ = accId;
    endResetModel();
}
