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

// LRC
#include "clientmessagesmodel.h"

ClientMessagesModel::ClientMessagesModel(Messages& msgList, QObject *parent)
    : QAbstractItemModel(parent),
      msgList_(std::move(msgList))
{
}

QModelIndex ClientMessagesModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (column != 0)
        return QModelIndex();

    if (row >= 0 && row < rowCount())
        return createIndex(row, column);

    return QModelIndex();
}

QModelIndex ClientMessagesModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

int ClientMessagesModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return msgList_.size();

    return 0;
}

int ClientMessagesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

QVariant ClientMessagesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
    case Role::Body:
        return QVariant(QString::fromStdString(msgList_.at(index.row()).body));
    case Role::IsOutgoing:
        return QVariant(msgList_.at(index.row()).is_outgoing);
    case Role::Timestamp:
        return QVariant(QDateTime::fromMSecsSinceEpoch(std::stol(msgList_.at(index.row()).timestamp)));
    default:
        return QVariant();
    }
}

ClientMessagesModel* ClientMessagesModel::fromMessages(Messages &msgList)
{
    return new ClientMessagesModel(msgList);
}
