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

#include "accountcomboboxqmlobjectholder.h"

#include "utils.h"

AccountComboBoxQmlObjectHolder::AccountComboBoxQmlObjectHolder(QObject* parent)
    : QObject(parent)
{
}

AccountComboBoxQmlObjectHolder::~AccountComboBoxQmlObjectHolder()
{
}

void
AccountComboBoxQmlObjectHolder::setAccountComboBoxQmlObject(QObject* obj)
{
    // Set the object pointer
    accountComboBoxQmlObject_ = obj;
}

void
AccountComboBoxQmlObjectHolder::accountChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (accountList.size() > index)
        setSelectedAccount(accountList.at(index), index);
}

void
AccountComboBoxQmlObjectHolder::setSelectedAccount(const QString& accountId, int index)
{
    LRCInstance::setSelectedAccountId(accountId);

    // First, we get back to the welcome view (except if in call)
    // in call?
    backToWelcomePage(index);

    auto& accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);

    QMetaObject::invokeMethod(accountComboBoxQmlObject_, "updateSmartList", Q_ARG(QVariant, accountId));
    
    currentTypeFilter_ = accountInfo.profileInfo.type;
    LRCInstance::getCurrentConversationModel()->setFilter(accountInfo.profileInfo.type);
    //updateConversationsFilterWidget();
    connectAccount(accountId);
}

void
AccountComboBoxQmlObjectHolder::backToWelcomePage(int index)
{
    deselectConversation();
    QMetaObject::invokeMethod(accountComboBoxQmlObject_, "backToWelcomePage", Q_ARG(QVariant, index));
}

void
AccountComboBoxQmlObjectHolder::deselectConversation()
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

void
AccountComboBoxQmlObjectHolder::connectAccount(const QString& accountId)
{
    /*try {
        auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
        QObject::disconnect(contactAddedConnection_);
        contactAddedConnection_ = QObject::connect(
            accInfo.contactModel.get(),
            &lrc::api::ContactModel::contactAdded,
            [this, accountId](const QString& contactUri) {
                auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
                auto convModel = LRCInstance::getCurrentConversationModel();
                auto conversation = LRCInstance::getCurrentConversation();
                if (conversation.uid.isEmpty()) {
                    return;
                }
                if (contactUri == accInfo.contactModel->getContact(conversation.participants.at(0)).profileInfo.uri) {
                    // update call screen
                    auto avatarImg = QPixmap::fromImage(imageForConv(conversation.uid));
                    ui->callingPhoto->setPixmap(avatarImg);
                    ui->callerPhoto->setPixmap(avatarImg);
                    // update conversation
                    ui->messageView->clear();
                    setConversationProfileData(conversation);
                    ui->messageView->printHistory(*convModel, conversation.interactions);
                }
            });
        QObject::disconnect(addedToConferenceConnection_);
        addedToConferenceConnection_ = QObject::connect(
            accInfo.callModel.get(),
            &NewCallModel::callAddedToConference,
            [this](const QString& callId, const QString& confId) {
                Q_UNUSED(callId);
                LRCInstance::renderer()->addDistantRenderer(confId);
                ui->videoView->updateCall();
            });
    }
    catch (...) {
        qWarning() << "Couldn't get account: " << accountId;
    }*/
}
