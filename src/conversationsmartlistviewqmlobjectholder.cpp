/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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
{

}

ConversationSmartListViewQmlObjectHolder::~ConversationSmartListViewQmlObjectHolder()
{
}

void
ConversationSmartListViewQmlObjectHolder::setConversationSmartListViewQmlObjectHolder(QObject* obj)
{
    conversationSmartListViewQmlObject_ = obj;

    conversationSmartListModel_ = new SmartListModel(LRCInstance::getCurrAccId(), this);

    QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "setModel", Q_ARG(QVariant, QVariant::fromValue(conversationSmartListModel_)));

    connectConversationModel();
}

void
ConversationSmartListViewQmlObjectHolder::backToWelcomePage()
{
    deselectConversation();
    QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "backToWelcomePage");
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

    //if (auto smartListSelectionModel = ui->smartList->selectionModel()) {
    //    smartListSelectionModel->clear();
    //}
}

bool
ConversationSmartListViewQmlObjectHolder::connectConversationModel()
{
    auto currentConversationModel = LRCInstance::getCurrentAccountInfo().conversationModel.get();

    /*if (ui->smartList->selectionModel()) {
        ui->smartList->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Deselect);
    }*/

    QObject::disconnect(modelSortedConnection_);
    QObject::disconnect(modelUpdatedConnection_);
    QObject::disconnect(filterChangedConnection_);
    QObject::disconnect(newConversationConnection_);
    QObject::disconnect(conversationRemovedConnection_);
    QObject::disconnect(conversationClearedConnection);
    QObject::disconnect(interactionStatusUpdatedConnection_);
    QObject::disconnect(newInteractionConnection_);
    QObject::disconnect(interactionRemovedConnection_);

    modelSortedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::modelSorted,
        [this]() {
            //updateConversationsFilterWidget();
            auto convUid = LRCInstance::getCurrentConversation().uid;
            auto convModel = LRCInstance::getCurrentConversationModel();
            auto& conversation = LRCInstance::getConversationFromConvUid(convUid);
            if (conversation.uid.isEmpty()) {
                return;
            }
            auto contactURI = conversation.participants[0];
            if (contactURI.isEmpty() ||
                convModel->owner.contactModel->getContact(contactURI).profileInfo.type == lrc::api::profile::Type::TEMPORARY) {
                return;
            }
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "modelSorted", Q_ARG(QVariant, contactURI));
        }
    );
    modelUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationUpdated,
        [this](const QString& convUid) {
            Q_UNUSED(convUid);
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "updateConversationSmartListView");
        }
    );
    filterChangedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::filterChanged,
        [this]() {
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "updateSmartList", Q_ARG(QVariant, LRCInstance::getCurrAccId()));
            //updateConversationsFilterWidget();
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "updateConversationSmartListView");
        }
    );
    newConversationConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::newConversation,
        [this](const QString& convUid) {
            QMetaObject::invokeMethod(conversationSmartListViewQmlObject_, "updateSmartList", Q_ARG(QVariant, LRCInstance::getCurrAccId()));
            //updateConversationForNewContact(convUid);
            //ui->conversationsFilterWidget->update();
            //ui->currentAccountComboBox->canPlaceAudioOnlyCall(convUid);
        }
    );
    conversationRemovedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationRemoved,
        [this]() {
            backToWelcomePage();
        }
    );
    conversationClearedConnection = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::conversationCleared,
        [this](const QString& convUid) {
            //ui->messageView->clear();
            // if currently selected,
            // switch to welcome screen (deselecting current smartlist item )
            if (convUid != LRCInstance::getCurrentConvUid()) {
                return;
            }
            backToWelcomePage();
        }
    );
    interactionStatusUpdatedConnection_ = QObject::connect(
        currentConversationModel, &lrc::api::ConversationModel::interactionStatusUpdated,
        [this](const QString& convUid, uint64_t interactionId, const lrc::api::interaction::Info& interaction) {
            if (convUid != LRCInstance::getCurrentConvUid()) {
                return;
            }
            auto& currentAccountInfo = LRCInstance::getCurrentAccountInfo();
            auto currentConversationModel = currentAccountInfo.conversationModel.get();
            currentConversationModel->clearUnreadInteractions(convUid);
            //ui->conversationsFilterWidget->update();
            //ui->messageView->updateInteraction(*currentConversationModel, interactionId, interaction);
        }
    );

    currentConversationModel->setFilter("");
    // clear search field
    //ui->ringContactLineEdit->setText("");
    return true;
}
