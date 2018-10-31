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

using InteractionInfo = lrc::api::interaction::Info;

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
    if (!index.isValid() || conv_.interactions.size() == 0) {
        return QVariant();
    }

    auto it = conv_.interactions.begin();
    std::advance(it, index.row());
    const auto& item = (*it).second;
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
        return QVariant(date);
    }
    case Role::Status:
        return QVariant::fromValue(static_cast<int>(item.status));
    case Role::Direction:
        return QVariant::fromValue(lrc::api::interaction::isOutgoing(item));
    case Role::Type:
        return QVariant::fromValue(static_cast<int>(item.type));
    case Role::Conversation:
        return QVariant::fromValue(conv_);
    case Role::Sequencing:
    {
        auto sequencingIt = sequencingCache_.find(index.row());
        if (sequencingIt == sequencingCache_.end()) {
            qDebug() << "sequencing entry not found in cache";
            auto newSequencing = computeSequencing(index);
            sequencingCache_.insert(std::pair<char, MsgSeq>(index.row(), newSequencing));
            return QVariant::fromValue(Utils::toUnderlyingValue(newSequencing));
        }
        return QVariant::fromValue(Utils::toUnderlyingValue((*sequencingIt).second));
    }
    }
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
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    } else {
        flags &= ~(Qt::ItemIsSelectable);
    }
    return flags;
}

MsgSeq
MessageModel::computeSequencing(const QModelIndex& index) const {
    auto conv = index
        .data(static_cast<int>(MessageModel::Role::Conversation))
        .value<MessageModel::ConversationInfo>();

    auto row = index.row();

    auto it = conv.interactions.begin();
    std::advance(it, row);
    auto interaction = it->second;

    if (interaction.type != lrc::api::interaction::Type::TEXT ||
        conv.interactions.size() == 1) {
        return MsgSeq::SINGLE_WITH_TIME;
    }

    if (row == 0) {
        auto end = conv.interactions.end();
        if (it == conv.interactions.end()) {
            return MsgSeq::SINGLE_WITH_TIME;
        }
        auto nextIt = it;
        nextIt++;
        auto nextInteraction = nextIt->second;
        if (sequenceChanged(interaction, nextInteraction)) {
            return MsgSeq::SINGLE_WITH_TIME;
        }
        return MsgSeq::FIRST_WITH_TIME;
    }

    if (row == rowCount() - 1) {
        if (it == conv.interactions.begin()) {
            return MsgSeq::SINGLE_WITH_TIME;
        }
        auto previousIt = it;
        previousIt--;
        auto previousInteraction = previousIt->second;
        bool timeChanged = sequenceTimeChanged(interaction, previousInteraction);
        bool authorChanged = sequenceAuthorChanged(interaction, previousInteraction);
        if (!timeChanged && !authorChanged) {
            return MsgSeq::LAST_IN_SEQUENCE;
        }
        if (!timeChanged && authorChanged) {
            return MsgSeq::SINGLE_WITHOUT_TIME;
        }
        return MsgSeq::SINGLE_WITH_TIME;
    }
    if (it == conv.interactions.begin() || it == conv.interactions.end()) {
        return MsgSeq::SINGLE_WITH_TIME;
    }

    auto previousIt = it;
    previousIt--;
    auto previousInteraction = previousIt->second;
    auto nextIt = it;
    nextIt++;
    auto nextInteraction = nextIt->second;

    bool timeChanged = sequenceTimeChanged(interaction, previousInteraction);
    bool authorChanged = sequenceAuthorChanged(interaction, previousInteraction);
    bool sequenceWillChange = sequenceChanged(interaction, nextInteraction);
    if (previousInteraction.type == lrc::api::interaction::Type::OUTGOING_DATA_TRANSFER ||
        previousInteraction.type == lrc::api::interaction::Type::INCOMING_DATA_TRANSFER) {
        if (!sequenceWillChange) {
            return MsgSeq::FIRST_WITH_TIME;
        }
        return MsgSeq::SINGLE_WITH_TIME;
    }
    if (!sequenceWillChange) {
        if (!timeChanged && !authorChanged) {
            return MsgSeq::MIDDLE_IN_SEQUENCE;
        }
        if (timeChanged) {
            return MsgSeq::FIRST_WITH_TIME;
        }
        return MsgSeq::FIRST_WITHOUT_TIME;
    } if (!timeChanged && !authorChanged) {
        return MsgSeq::LAST_IN_SEQUENCE;
    } if (timeChanged) {
        return MsgSeq::SINGLE_WITH_TIME;
    }
    return MsgSeq::SINGLE_WITHOUT_TIME;
}

bool
MessageModel::sequenceChanged(InteractionInfo& firstInteraction, InteractionInfo& secondInteraction) const {
    return (sequenceTimeChanged(firstInteraction, secondInteraction) || sequenceAuthorChanged(firstInteraction, secondInteraction));
}

bool
MessageModel::sequenceTimeChanged(InteractionInfo& firstInteraction, InteractionInfo& secondInteraction) const {
    bool timeChanged = false;
   /* NSDate* firstMessageTime = [NSDate dateWithTimeIntervalSince1970 : firstInteraction.timestamp];
    NSDate* secondMessageTime = [NSDate dateWithTimeIntervalSince1970 : secondInteraction.timestamp];
    bool hourComp = [[NSCalendar currentCalendar] compareDate:firstMessageTime toDate : secondMessageTime toUnitGranularity : NSCalendarUnitHour];
    bool minutComp = [[NSCalendar currentCalendar] compareDate:firstMessageTime toDate : secondMessageTime toUnitGranularity : NSCalendarUnitMinute];
    if (hourComp != NSOrderedSame || minutComp != NSOrderedSame) {
        timeChanged = true;
    }*/
    return timeChanged;
}

bool
MessageModel::sequenceAuthorChanged(InteractionInfo& firstInteraction, InteractionInfo& secondInteraction) const {
    bool authorChanged = true;
    bool isOutgoing = lrc::api::interaction::isOutgoing(firstInteraction);
    if ((secondInteraction.type == lrc::api::interaction::Type::TEXT) && (isOutgoing == lrc::api::interaction::isOutgoing(secondInteraction))) {
        authorChanged = false;
    }
    return authorChanged;
}

const std::string&
MessageModel::timeForMessage(time_t msgTime) const {
    /* NSDate *today = [NSDate date];
     NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
     [dateFormatter setLocale : [[NSLocale alloc] initWithLocaleIdentifier:[[NSLocale currentLocale] localeIdentifier]]];
     if ([[NSCalendar currentCalendar] compareDate:today
         toDate : msgTime
         toUnitGranularity : NSCalendarUnitYear] != NSOrderedSame) {
         return[NSDateFormatter localizedStringFromDate : msgTime dateStyle : NSDateFormatterMediumStyle timeStyle : NSDateFormatterMediumStyle];
     }

     if ([[NSCalendar currentCalendar] compareDate:today
         toDate : msgTime
         toUnitGranularity : NSCalendarUnitDay] != NSOrderedSame ||
         [[NSCalendar currentCalendar] compareDate:today
         toDate : msgTime
         toUnitGranularity : NSCalendarUnitMonth] != NSOrderedSame) {
         [dateFormatter setDateFormat : @"MMM dd, HH:mm"];
             return[dateFormatter stringFromDate : msgTime];
     }

     [dateFormatter setDateFormat : @"HH:mm"];
         return[dateFormatter stringFromDate : msgTime];*/
    return "";
}