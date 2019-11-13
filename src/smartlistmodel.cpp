/***************************************************************************
 * Copyright (C) 2017-2019 by Savoir-faire Linux                           *
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "pixbufmanipulator.h"
#include "utils.h"
#include "lrcinstance.h"

#include "globalinstances.h"
#include "api/contactmodel.h"

#include <QDateTime>

SmartListModel::SmartListModel(const std::string& accId,
                               QObject *parent,
                               SmartListModel::Type listModelType,
                               const std::string& convUid)
    : QAbstractItemModel(parent)
    , accountId_(accId)
    , listModelType_(listModelType)
    , convUid_(convUid)
{
    if (listModelType_ == Type::CONFERENCE) {
        setConferenceableFilter();
    }
}

int SmartListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId_);
        auto& convModel = accInfo.conversationModel;
        if (listModelType_ == Type::TRANSFER) {
            auto filterType = accInfo.profileInfo.type;
            return convModel->getFilteredConversations(filterType).size();
        } else if (listModelType_ == Type::CONFERENCE) {
            auto calls = conferenceables_.at(ConferenceableItem::CALL);
            auto contacts = conferenceables_.at(ConferenceableItem::CONTACT);
            auto rowCount = contacts.size();
            if (calls.size()) {
                rowCount = 2;
                rowCount += sectionState_[tr("Calls")] ? calls.size() : 0;
                rowCount += sectionState_[tr("Contacts")] ? contacts.size() : 0;
            }
            return rowCount;
        }
        return accInfo.conversationModel->allFilteredConversations().size();
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

    try {
        auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId_);
        auto& convModel = accountInfo.conversationModel;
        lrc::api::conversation::Info item;
        if (listModelType_ == Type::TRANSFER) {
            auto filterType = accountInfo.profileInfo.type;
            item = convModel->getFilteredConversations(filterType).at(index.row());
            return getConversationItemData(item, accountInfo, role);
        } else if (listModelType_ == Type::CONFERENCE) {
            auto calls = conferenceables_.at(ConferenceableItem::CALL);
            auto contacts = conferenceables_.at(ConferenceableItem::CONTACT);
            std::string itemConvUid{}, itemAccId{};
            if (calls.size() == 0) {
                itemConvUid = contacts.at(index.row()).at(0).convId;
                itemAccId = contacts.at(index.row()).at(0).accountId;
            } else {
                bool callsOpen = sectionState_[tr("Calls")];
                bool contactsOpen = sectionState_[tr("Contacts")];
                auto callSectionEnd = callsOpen ? calls.size() + 1 : 1;
                auto contactSectionEnd = contactsOpen ?
                    callSectionEnd + contacts.size() + 1 :
                    callSectionEnd + 1;
                if (index.row() < callSectionEnd) {
                    if (index.row() == 0) {
                        return QVariant(role == Role::SectionName ?
                            (callsOpen ? "➖ " : "➕ ") + QString(tr("Calls")) :
                            "");
                    } else {
                        auto idx = index.row() - 1;
                        itemConvUid = calls.at(idx).at(0).convId;
                        itemAccId = calls.at(idx).at(0).accountId;
                    }
                } else if (index.row() < contactSectionEnd) {
                    if (index.row() == callSectionEnd) {
                        return QVariant(role == Role::SectionName ?
                            (contactsOpen ? "➖ " : "➕ ") + QString(tr("Contacts")) :
                            "");
                    } else {
                        auto idx = index.row() - (callSectionEnd + 1);
                        itemConvUid = contacts.at(idx).at(0).convId;
                        itemAccId = contacts.at(idx).at(0).accountId;
                    }
                }
            }
            if (role == Role::AccountId) {
                return QVariant(QString::fromStdString(itemAccId));
            }
            item = LRCInstance::getConversationFromConvUid(itemConvUid, itemAccId);
            auto& itemAccountInfo = LRCInstance::accountModel().getAccountInfo(itemAccId);
            return getConversationItemData(item, itemAccountInfo, role);
        } else if (listModelType_ == Type::CONVERSATION) {
            item = convModel->filteredConversation(index.row());
            return getConversationItemData(item, accountInfo, role);
        }
    } catch (const std::exception& e) {
        qWarning() << e.what();
    }
    return QVariant();
}

void
SmartListModel::setConferenceableFilter(const std::string& filter)
{
    beginResetModel();
    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId_);
    auto& convModel = accountInfo.conversationModel;
    conferenceables_ = convModel->getConferenceableConversations(convUid_, filter);
    sectionState_[tr("Calls")] = true;
    sectionState_[tr("Contacts")] = true;
    endResetModel();
}

void
SmartListModel::toggleSection(const QString& section)
{
    beginResetModel();
    if (section.contains(tr("Calls"))) {
        sectionState_[tr("Calls")] ^= true;
    } else if (section.contains(tr("Contacts"))) {
        sectionState_[tr("Contacts")] ^= true;
    }
    endResetModel();
}

QVariant
SmartListModel::getConversationItemData(const conversation::Info& item,
                                        const account::Info& accountInfo,
                                        int role) const
{
    if (item.participants.size() <= 0) {
        return QVariant();
    }
    auto& contactModel = accountInfo.contactModel;
    switch (role) {
    case Role::Picture:
    case Qt::DecorationRole:
        return GlobalInstances::pixmapManipulator().decorationRole(item, accountInfo);
    case Role::DisplayName:
    case Qt::DisplayRole:
    {
        auto& contact = contactModel->getContact(item.participants[0]);
        return QVariant(QString::fromStdString(Utils::bestNameForContact(contact)));
    }
    case Role::DisplayID:
    {
        auto& contact = contactModel->getContact(item.participants[0]);
        return QVariant(QString::fromStdString(Utils::bestIdForContact(contact)));
    }
    case Role::Presence:
    {
        auto& contact = contactModel->getContact(item.participants[0]);
        return QVariant(contact.isPresent);
    }
    case Role::URI:
    {
        auto& contact = contactModel->getContact(item.participants[0]);
        return QVariant(QString::fromStdString(contact.profileInfo.uri));
    }
    case Role::UnreadMessagesCount:
        return QVariant(item.unreadMessages);
    case Role::LastInteractionDate:
    {
        auto& date = item.interactions.at(item.lastMessageUid).timestamp;
        return QVariant(QString::fromStdString(Utils::formatTimeString(date)));
    }
    case Role::LastInteraction:
        return QVariant(QString::fromStdString(item.interactions.at(item.lastMessageUid).body));
    case Role::LastInteractionType:
        return QVariant(Utils::toUnderlyingValue(item.interactions.at(item.lastMessageUid).type));
    case Role::ContactType:
    {
        auto& contact = contactModel->getContact(item.participants[0]);
        return QVariant(Utils::toUnderlyingValue(contact.profileInfo.type));
    }
    case Role::UID:
        return QVariant(QString::fromStdString(item.uid));
    case Role::ContextMenuOpen:
        return QVariant(isContextMenuOpen);
    case Role::InCall:
    {
        auto& convInfo = LRCInstance::getConversationFromConvUid(item.uid);
        if (!convInfo.uid.empty()) {
            auto callModel = LRCInstance::getCurrentCallModel();
            return QVariant(callModel->hasCall(convInfo.callId));
        }
        return QVariant(false);
    }
    case Role::CallStateStr:
    {
        auto& convInfo = LRCInstance::getConversationFromConvUid(item.uid);
        if (!convInfo.uid.empty()) {
            auto call = LRCInstance::getCallInfoForConversation(convInfo);
            if (call) {
                auto statusString = call::to_string(call->status);
                return QVariant(QString::fromStdString(statusString));
            }
        }
        return QVariant();
    }
    case Role::SectionName:
        return QVariant(QString());
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
    auto flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable;
    auto type = Utils::toEnum<lrc::api::profile::Type>(data(index, Role::ContactType).value<int>());
    auto uid = data(index, Role::UID).value<QString>();
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    } else if ( (type == lrc::api::profile::Type::TEMPORARY &&
                uid.isEmpty())) {
        flags &= ~(Qt::ItemIsSelectable);
    }
    return flags;
}

void
SmartListModel::setAccount(const std::string& accountId)
{
    beginResetModel();
    accountId_ = accountId;
    endResetModel();
}
