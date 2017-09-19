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
// Qt
#include <QDateTime>

// Client
#include "messagemodel.h"

MessageModel::MessageModel(const lrc::api::conversation::Info &conv, QObject *parent)
    : QAbstractItemModel(parent),
      conv_(conv)
{
}

QModelIndex MessageModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (column != 0)
        return QModelIndex();

    auto it = conv_.messages.find(row);
    if (it != conv_.messages.end())
        return createIndex(row, column);

    return QModelIndex();
}

QModelIndex MessageModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return conv_.messages.size();

    return 0;
}

int MessageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
    case Role::Body:
        return QVariant(QString::fromStdString(conv_.messages.at(index.row()).body));
    case Role::IsOutgoing:
        return QVariant(conv_.messages.at(index.row()).status != lrc::api::message::Status::READ);
    case Role::Timestamp:
        return QVariant(QDateTime::fromTime_t(conv_.messages.at(index.row()).timestamp));
    default:
        return QVariant();
    }
}
