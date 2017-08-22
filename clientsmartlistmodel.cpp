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
#include "clientsmartlistmodel.h"
#include "contactitem.h"
#include "smartlistmodel.h"
#include "globalinstances.h"

// Client
#include "pixbufmanipulator.h"

ClientSmartListModel::ClientSmartListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    auto& smlInstance = SmartListModel::instance();

    // Setup QAbstractItemModel signals
    connect(&smlInstance, &SmartListModel::modelUpdated, [this] {
        emit layoutChanged();
    });
    connect(&smlInstance, &SmartListModel::itemChanged, [this] (unsigned int row) {
        auto element = createIndex(row, 0);
        emit dataChanged(element, element);
    });

    // Setup LRC's SmartListModel signals
    connect(&smlInstance, &SmartListModel::showConversationView, [this] (SmartListItem* item) {
        emit showConversationView(createIndex(-1, 0, item)); // TODO Add correct row (not possible at current state of LRC)
    });
}

ClientSmartListModel& ClientSmartListModel::instance()
{
    static auto* obj = new ClientSmartListModel();
    return *obj;
}

int ClientSmartListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return SmartListModel::instance().getItems().size(); // The only method to get row count as of current state of LRC

    return 0; // A valid QModelIndex returns 0 as no entry has sub-elements
}

int ClientSmartListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 1;
}

QVariant ClientSmartListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // If index contains a pointer, it has priority over getting data by position in the list
    SmartListItem* item = reinterpret_cast<SmartListItem*>(index.internalPointer());
    if (!item)
        item = SmartListModel::instance().getItem(index.row()).get();

    if (item) {
        switch (role) {
        case Role::Picture:
        case Qt::DecorationRole:
        {
            auto& pbm = GlobalInstances::pixmapManipulator();
            return pbm.decorationRole(item);
        }
        case Role::DisplayName:
        case Qt::DisplayRole:
            return QVariant(QString::fromStdString(item->getTitle()));
        case Role::DisplayID:
            // TODO
            return QVariant(QString());
        case Role::Presence:
            return QVariant(item->isPresent());
        case Role::URI:
            if (auto contact = dynamic_cast<ContactItem*>(item))
                return QVariant(QString::fromStdString(contact->getUri()));
            else
                return QVariant();
        case Role::UnreadMessagesCount:
            // TODO
            return QVariant(0);
        case Role::LastInteractionDate:
            return QVariant(QDateTime::fromMSecsSinceEpoch(item->getLastInteractionTimeStamp()));
        case Role::LastInteraction:
            return QVariant(QString::fromStdString(item->getLastInteraction()));
        case Role::Messages:
        {
            auto history = item->getHistory();
            return QVariant::fromValue(static_cast<void*>(ClientMessagesModel::fromMessages(history)));
        }
        }
    }

    return QVariant();
}

QModelIndex ClientSmartListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    if (column != 0)
        return QModelIndex();

    if (row >= 0 && row < rowCount())
        return createIndex(row, column, SmartListModel::instance().getItem(row).get());

    return QModelIndex();
}

QModelIndex ClientSmartListModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

Qt::ItemFlags ClientSmartListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);
    else
        return QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;
}

void ClientSmartListModel::setFilter(QString& filter)
{
    SmartListModel::instance().setFilter(filter.toStdString());
}
