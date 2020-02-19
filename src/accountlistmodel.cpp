/***************************************************************************
* Copyright (C) 2019-2019 by Savoir-faire Linux                                 *
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

#include "accountlistmodel.h"

// Qt
#include <QDateTime>

// LRC
#include "globalinstances.h"

// Client
#include "pixbufmanipulator.h"
#include "lrcinstance.h"
#include "utils.h"

AccountListModel::AccountListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

int AccountListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return LRCInstance::accountModel().getAccountList().size() + 1; // count
    }
    return 0; // A valid QModelIndex returns 0 as no entry has sub-elements
}

int AccountListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant AccountListModel::data(const QModelIndex &index, int role) const
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (!index.isValid() || accountList.size() <= index.row()) {
        return QVariant();
    }

    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountList.at(index.row()));

    switch (role) {
    case Role::Alias:
    case Qt::DisplayRole:
        return QVariant(Utils::bestNameForAccount(accountInfo));
    case Role::Username:
        return QVariant(Utils::secondBestNameForAccount(accountInfo));
    case Role::Type:
        return QVariant(Utils::toUnderlyingValue<lrc::api::profile::Type>(accountInfo.profileInfo.type));
    case Role::Status:
        return QVariant(Utils::toUnderlyingValue<lrc::api::account::Status>(accountInfo.status));
    case Role::Picture:
    case Qt::DecorationRole:
        return Utils::accountPhoto(accountInfo);
    case Role::ID:
        return QVariant(accountInfo.id);
    }
    return QVariant();
}

QModelIndex AccountListModel::index(int row, int column, const QModelIndex &parent) const
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

QModelIndex AccountListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

Qt::ItemFlags AccountListModel::flags(const QModelIndex &index) const
{
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return flags;
}
