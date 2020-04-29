/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bannedlistmodel.h"
#include "lrcinstance.h"

BannedListModel::BannedListModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int
BannedListModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return LRCInstance::accountModel().getAccountList().size(); // count
    }
    return 0; // A valid QModelIndex returns 0 as no entry has sub-elements
}

bool
BannedListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return 1;
}