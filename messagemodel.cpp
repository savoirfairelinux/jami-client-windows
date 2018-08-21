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

#include "messagemodel.h"

// Qt
#include <QDateTime>

// LRC
#include "globalinstances.h"
#include "api/contactmodel.h"
#include "api/conversationmodel.h"

// Client
#include "pixbufmanipulator.h"
#include "utils.h"

Q_DECLARE_METATYPE(lrc::api::interaction::Status);

MessageModel::MessageModel(const ConversationInfo& conv, const AccountInfo& acc, QObject *parent)
    : QAbstractItemModel(parent),
    conv_(conv),
    acc_(acc)
{
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return conv_.interactions.size();
    }
    return 0; // A valid QModelIndex returns 0 as no entry has sub-elements
}

int MessageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    try {
        const auto& item = conv_.interactions.at(index.row());
        switch (role) {
        case Role::Body:
            return QVariant(QString::fromStdString(item.body));
        case Role::Picture:
        case Qt::DecorationRole:
            return GlobalInstances::pixmapManipulator().decorationRole(conv_, acc_);
        case Role::DisplayName:
        case Qt::DisplayRole:
        {
            auto& contact = acc_.contactModel->getContact(conv_.participants[0]);
            return QVariant(QString::fromStdString(Utils::bestNameForContact(contact)));
        }
        case Role::Presence:
        {
            auto& contact = acc_.contactModel->getContact(conv_.participants[0]);
            return QVariant(contact.isPresent);
        }
        case Role::InteractionDate:
        {
            auto& date = item.timestamp;
            return QVariant(QDateTime::fromTime_t(date));
        }
        case Role::Status:
            return QVariant::fromValue(item.status);
        case Role::Direction:
            return QVariant::fromValue(lrc::api::interaction::isOutgoing(item));
        case Role::Type:
            return QVariant::fromValue(static_cast<int>(item.type));
        }
    }
    catch (...) {}
    return QVariant();
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex MessageModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    else {
        return QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;
    }
}

void MessageModel::selectConversation(const QModelIndex &current, const QModelIndex &previous)
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