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

// Client
#include "clientaccountmodel.h"
#include "smartlistmodel.h"

// LRC
#include "api/newaccountmodel.h"
#include "api/account.h"

ClientAccountModel::ClientAccountModel(const lrc::api::NewAccountModel& mdl, QObject *parent)
    : QAbstractItemModel(parent),
      mdl_(mdl)
{}

QModelIndex ClientAccountModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
        return QModelIndex();

    if(column != 0)
        return QModelIndex();

    if(row >= 0 && row < rowCount())
        return createIndex(row, column);

    return QModelIndex();
}

QModelIndex ClientAccountModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)

    return QModelIndex();
}

int ClientAccountModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return mdl_.getAccountList().size();

    return 0;
}

int ClientAccountModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    return 0;
}

QVariant ClientAccountModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto& accInfo = mdl_.getAccountInfo(mdl_.getAccountList().at(index.row()));

    switch(role) {
    case Qt::DisplayRole:
    case Role::Alias:
        // TODO Display account alias instead of id
        return QVariant(QString::fromStdString(accInfo.id));
    case Role::SmartListModel:
        return QVariant::fromValue<::SmartListModel*>(new ::SmartListModel(accInfo));
    }

    return QVariant();
}

Qt::ItemFlags ClientAccountModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);
    else
        return QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;
}

std::vector<std::__cxx11::string> ClientAccountModel::getAccountList() const
{
    return mdl_.getAccountList();
}

const lrc::api::account::Info &ClientAccountModel::getAccountInfo(const std::__cxx11::string &accountId) const
{
    return mdl_.getAccountInfo(accountId);
}
