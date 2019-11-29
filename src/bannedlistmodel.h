/***************************************************************************
* Copyright (C) 2019-2019 by Savoir-faire Linux                                 *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                       *
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
#pragma once

#include <QAbstractListModel>

class BannedListModel : public QAbstractListModel
{
    Q_OBJECT
    BannedListModel(const BannedListModel& cpy);

public:
    explicit BannedListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

};
