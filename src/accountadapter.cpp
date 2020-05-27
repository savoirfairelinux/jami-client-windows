/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>
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

#include "accountadapter.h"

#include "lrcinstance.h"
#include "utils.h"

AccountAdapter::AccountAdapter(QObject *parent)
    : QmlAdapterBase(parent)
{}

AccountAdapter::~AccountAdapter() {}

void
AccountAdapter::initQmlObject()
{
    connectAccount(LRCInstance::getCurrAccId());
}

void
AccountAdapter::accountChanged(int index)
{
    auto accountList = LRCInstance::accountModel().getAccountList();
    if (accountList.size() > index)
        setSelectedAccount(accountList.at(index), index);
}

void
AccountAdapter::setSelectedAccount(const QString &accountId, int index)
{
    LRCInstance::setSelectedAccountId(accountId);

    backToWelcomePage(index);

    auto &accountInfo = LRCInstance::accountModel().getAccountInfo(accountId);

    QMetaObject::invokeMethod(qmlObj_, "updateSmartList", Q_ARG(QVariant, accountId));
    connectAccount(accountId);
    emit accountSignalsReconnect(accountId);
    //emit slotAccountChangedFinished();
}

void
AccountAdapter::backToWelcomePage(int index)
{
    deselectConversation();
    QMetaObject::invokeMethod(qmlObj_, "backToWelcomePage", Q_ARG(QVariant, index));
}

void
AccountAdapter::deselectConversation()
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
AccountAdapter::connectAccount(const QString &accountId)
{
    try {
        auto &accInfo = LRCInstance::accountModel().getAccountInfo(accountId);

        QObject::disconnect(accountStatusChangedConnection_);
        QObject::disconnect(contactAddedConnection_);

        accountStatusChangedConnection_
            = QObject::connect(accInfo.accountModel,
                               &lrc::api::NewAccountModel::accountStatusChanged,
                               [this] { emit accountStatusChanged(); });

        contactAddedConnection_
            = QObject::connect(accInfo.contactModel.get(),
                               &lrc::api::ContactModel::contactAdded,
                               [this, accountId](const QString &contactUri) {
                                   auto &accInfo = LRCInstance::accountModel().getAccountInfo(
                                       accountId);
                                   auto convModel = LRCInstance::getCurrentConversationModel();
                                   auto conversation = LRCInstance::getCurrentConversation();
                                   if (conversation.uid.isEmpty()) {
                                       return;
                                   }
                                   if (contactUri
                                       == accInfo.contactModel
                                              ->getContact(conversation.participants.at(0))
                                              .profileInfo.uri) {
                                       /*
                                        * Update conversation.
                                        */
                                       emit updateConversationForAddedContact();
                                   }
                               });
        QObject::disconnect(addedToConferenceConnection_);
        addedToConferenceConnection_
            = QObject::connect(accInfo.callModel.get(),
                               &NewCallModel::callAddedToConference,
                               [this](const QString &callId, const QString &confId) {
                                   Q_UNUSED(callId);
                                   LRCInstance::renderer()->addDistantRenderer(confId);
                               });
    } catch (...) {
        qWarning() << "Couldn't get account: " << accountId;
    }
}
