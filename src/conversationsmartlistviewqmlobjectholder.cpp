/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "conversationsmartlistviewqmlobjectholder.h"

#include "utils.h"

ConversationSmartListViewQmlObjectHolder::ConversationSmartListViewQmlObjectHolder(QObject *parent)
    : QObject(parent)
{}

ConversationSmartListViewQmlObjectHolder::~ConversationSmartListViewQmlObjectHolder() {}

void
ConversationSmartListViewQmlObjectHolder::setConversationSmartListViewQmlObjectHolder(QObject *obj)
{
    conversationSmartListViewQmlObject_ = obj;

    conversationSmartListModel_ = new SmartListModel(LRCInstance::getCurrAccId(), this);

    QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                              "setModel",
                              Q_ARG(QVariant, QVariant::fromValue(conversationSmartListModel_)));

    connect(&LRCInstance::behaviorController(),
            &BehaviorController::showChatView,
            [this](const QString &accountId, lrc::api::conversation::Info convInfo) {
                emit showChatView(accountId, convInfo.uid);
            });

    connectConversationModel();
}

void
ConversationSmartListViewQmlObjectHolder::backToWelcomePage()
{
    deselectConversation();
    QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "backToWelcomePage");
}

void
ConversationSmartListViewQmlObjectHolder::selectConversation(const QString &accountId,
                                                             const QString &convUid)
{
    selectConversation(LRCInstance::getConversationFromConvUid(convUid, accountId), true);
}

void
ConversationSmartListViewQmlObjectHolder::selectConversation(int index)
{
    auto convModel = LRCInstance::getCurrentConversationModel();

    if (convModel == nullptr) {
        return;
    }

    const auto item = convModel->filteredConversation(index);

    if (selectConversation(item)) {
        auto convUid = conversationSmartListModel_
                           ->data(conversationSmartListModel_->index(index, 0),
                                  static_cast<int>(SmartListModel::Role::UID))
                           .toString();
        auto &conversation = LRCInstance::getConversationFromConvUid(convUid);
        // is calling, show callview(can use showChatView signal, since it will be determined on qml)
        if (!conversation.uid.isEmpty()
            && LRCInstance::getCurrentCallModel()->hasCall(conversation.callId)) {
            emit showChatView(LRCInstance::getCurrAccId(), conversation.uid);
        }
    }
}

bool
ConversationSmartListViewQmlObjectHolder::selectConversation(
    const lrc::api::conversation::Info &item, bool preventSendingSignal)
{
    if (LRCInstance::getCurrentConvUid() == item.uid) {
        return false;
    } else if (item.participants.size() > 0) {
        auto &accInfo = LRCInstance::getAccountInfo(item.accountId);
        LRCInstance::setSelectedConvId(item.uid);
        if (!preventSendingSignal)
            accInfo.conversationModel->selectConversation(item.uid);
        accInfo.conversationModel->clearUnreadInteractions(item.uid);
        //ui->conversationsFilterWidget->update();
        return true;
    }
}

void
ConversationSmartListViewQmlObjectHolder::deselectConversation()
{
    if (LRCInstance::getCurrentConvUid().isEmpty()) {
        return;
    }

    auto currentConversationModel = LRCInstance::getCurrentConversationModel();

    if (currentConversationModel == nullptr) {
        return;
    }

    currentConversationModel->selectConversation("");
    LRCInstance::setSelectedConvId();
}

void
ConversationSmartListViewQmlObjectHolder::accountChangedSetUp(const QString &accountId)
{
    auto &accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);
    currentTypeFilter_ = accountInfo.profileInfo.type;
    LRCInstance::getCurrentConversationModel()->setFilter(accountInfo.profileInfo.type);
    updateConversationsFilterWidget();

    connectConversationModel();
}

void
ConversationSmartListViewQmlObjectHolder::updateConversationsFilterWidget()
{
    auto invites = LRCInstance::getCurrentAccountInfo().contactModel->pendingRequestCount();
    if (invites == 0 && currentTypeFilter_ == lrc::api::profile::Type::PENDING) {
        currentTypeFilter_ = lrc::api::profile::Type::RING;
        LRCInstance::getCurrentConversationModel()->setFilter(currentTypeFilter_);
    }
    showConversationTabs(invites);
}

void
ConversationSmartListViewQmlObjectHolder::setConversationFilter(const QString &type)
{
    if (type.isEmpty()) {
        if (LRCInstance::getCurrentAccountInfo().profileInfo.type == lrc::api::profile::Type::RING)
            setConversationFilter(lrc::api::profile::Type::RING);
        else
            setConversationFilter(lrc::api::profile::Type::SIP);
    } else {
        setConversationFilter(lrc::api::profile::to_type(type));
    }
}

void
ConversationSmartListViewQmlObjectHolder::setConversationFilter(lrc::api::profile::Type filter)
{
    if (currentTypeFilter_ == filter) {
        return;
    }
    currentTypeFilter_ = filter;
    LRCInstance::getCurrentConversationModel()->setFilter(currentTypeFilter_);
}

bool
ConversationSmartListViewQmlObjectHolder::connectConversationModel()
{
    auto currentConversationModel = LRCInstance::getCurrentAccountInfo().conversationModel.get();

    QObject::disconnect(modelSortedConnection_);
    QObject::disconnect(modelUpdatedConnection_);
    QObject::disconnect(filterChangedConnection_);
    QObject::disconnect(newConversationConnection_);
    QObject::disconnect(conversationRemovedConnection_);
    QObject::disconnect(conversationClearedConnection);
    QObject::disconnect(newInteractionConnection_);
    QObject::disconnect(interactionRemovedConnection_);

    modelSortedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::modelSorted, [this]() {
            updateConversationsFilterWidget();
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                      "updateConversationSmartListView");
            auto convUid = LRCInstance::getCurrentConversation().uid;
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto &conversation = LRCInstance::getConversationFromConvUid(convUid);
            if (conversation.uid.isEmpty()) {
                return;
            }
            auto contactURI = conversation.participants[0];
            if (contactURI.isEmpty()
                || convModel->owner.contactModel->getContact(contactURI).profileInfo.type
                       == lrc::api::profile::Type::TEMPORARY) {
                return;
            }
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                      "modelSorted",
                                      Q_ARG(QVariant, contactURI));
        });

    modelUpdatedConnection_
        = QObject::connect(currentConversationModel,
                           &lrc::api::ConversationModel::conversationUpdated,
                           [this](const QString &convUid) {
                               Q_UNUSED(convUid);
                               updateConversationsFilterWidget();
                               QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                                         "updateConversationSmartListView");
                           });

    filterChangedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::filterChanged, [this]() {
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                      "updateSmartList",
                                      Q_ARG(QVariant, LRCInstance::getCurrAccId()));
            updateConversationsFilterWidget();
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                      "updateConversationSmartListView");
        });

    newConversationConnection_
        = QObject::connect(currentConversationModel,
                           &lrc::api::ConversationModel::newConversation,
                           [this](const QString &convUid) {
                               QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                                         "updateSmartList",
                                                         Q_ARG(QVariant,
                                                               LRCInstance::getCurrAccId()));
                               updateConversationForNewContact(convUid);
                               //ui->conversationsFilterWidget->update();
                               //ui->currentAccountComboBox->canPlaceAudioOnlyCall(convUid);
                           });

    conversationRemovedConnection_
        = QObject::connect(currentConversationModel,
                           &lrc::api::ConversationModel::conversationRemoved,
                           [this]() { backToWelcomePage(); });

    conversationClearedConnection
        = QObject::connect(currentConversationModel,
                           &lrc::api::ConversationModel::conversationCleared,
                           [this](const QString &convUid) {
                               //ui->messageView->clear();
                               // if currently selected,
                               // switch to welcome screen (deselecting current smartlist item )
                               if (convUid != LRCInstance::getCurrentConvUid()) {
                                   return;
                               }
                               backToWelcomePage();
                           });

    newInteractionConnection_
        = QObject::connect(currentConversationModel,
                           &lrc::api::ConversationModel::newInteraction,
                           [this] {
                               updateConversationsFilterWidget();
                               QMetaObject::invokeMethod(conversationSmartListViewQmlObject_,
                                                         "updateConversationSmartListView");
                           });

    currentConversationModel->setFilter("");
    // clear search field
    //ui->ringContactLineEdit->setText("");
    return true;
}

void
ConversationSmartListViewQmlObjectHolder::updateConversationForNewContact(const QString &convUid)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    if (convModel == nullptr) {
        return;
    }
    auto selectedUid = LRCInstance::getCurrentConvUid();
    auto &conversation = LRCInstance::getConversationFromConvUid(convUid, {}, true);
    if (!conversation.uid.isEmpty()) {
        try {
            auto contact = convModel->owner.contactModel->getContact(conversation.participants[0]);
            if (!contact.profileInfo.uri.isEmpty() && contact.profileInfo.uri == selectedUid) {
                LRCInstance::setSelectedConvId(convUid);
                convModel->selectConversation(convUid);
            }
        } catch (...) {
            return;
        }
    }
}