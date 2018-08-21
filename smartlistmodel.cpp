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

#include "smartlistmodel.h"

// Qt
#include <QDateTime>
#include <QSortFilterProxyModel>

// LRC
#include "globalinstances.h"
#include "api/contactmodel.h"
#include "api/conversationmodel.h"

// Client
#include "pixbufmanipulator.h"

SmartListModel::SmartListModel(const lrc::api::account::Info &acc, QObject *parent)
    : QAbstractItemModel(parent),
    acc_(acc)
{
    if (acc_.profileInfo.type == lrc::api::profile::Type::RING) {
        acc_.conversationModel->setFilter(lrc::api::profile::Type::RING);
    } else {
        acc_.conversationModel->setFilter(lrc::api::profile::Type::SIP);
    }

    connect(acc_.conversationModel.get(), &lrc::api::ConversationModel::modelSorted,
        [this] {
            emit dataChanged(index(0, 0), index(rowCount(), 0));
        });

    connect(acc_.conversationModel.get(), &lrc::api::ConversationModel::filterChanged,
        [this] {
            emit layoutChanged();
        });
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
                // TODO Handle conversations with multiple contacts
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                if (!contact.profileInfo.alias.empty())
                    return QVariant(QString::fromStdString(contact.profileInfo.alias));
                else if (!contact.registeredName.empty())
                    return QVariant(QString::fromStdString(contact.registeredName));
                else
                    return QVariant(QString::fromStdString(contact.profileInfo.uri));
            }
            case Role::DisplayID:
            {
                // TODO Handle conversations with multiple contacts
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                if (!contact.registeredName.empty())
                    return QVariant(QString::fromStdString(contact.registeredName));
                else
                    return QVariant(QString::fromStdString(contact.profileInfo.uri));
            }
            case Role::Presence:
            {
                // TODO Handle conversations with multiple contacts
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(contact.isPresent);
            }
            case Role::URI:
            {
                // TODO Handle conversations with multiple contacts
                auto& contact = acc_.contactModel->getContact(item.participants[0]);
                return QVariant(QString::fromStdString(contact.profileInfo.uri));
            }
            case Role::UnreadMessagesCount:
                return QVariant(item.unreadMessages);
                case Role::LastInteractionDate:
                {
                    auto& date = item.interactions.at(item.lastMessageUid).timestamp;
                    return QVariant(QDateTime::fromTime_t(date));
                }
                case Role::LastInteraction:
                    return QVariant(QString::fromStdString(item.interactions.at(item.lastMessageUid).body));
                /*case Role::Messages:
                return QVariant::fromValue(reinterpret_cast<void*>(new MessageModel(item, acc_)));*/
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
    //    emit layoutChanged();
}

void SmartListModel::selectConversation(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if (!current.isValid()) {
            return;
    }
    const auto item = acc_.conversationModel->filteredConversation(current.row());
    if (item.participants.size() > 0) {
        acc_.conversationModel->selectConversation(item.uid);
    }
}