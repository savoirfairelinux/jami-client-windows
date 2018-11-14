/**************************************************************************
| Copyright (C) 2015-2018 by Savoir-faire Linux                           |
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          |
|                                                                         |
| This program is free software; you can redistribute it and/or modify    |
| it under the terms of the GNU General Public License as published by    |
| the Free Software Foundation; either version 3 of the License, or       |
| (at your option) any later version.                                     |
|                                                                         |
| This program is distributed in the hope that it will be useful,         |
| but WITHOUT ANY WARRANTY; without even the implied warranty of          |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           |
| GNU General Public License for more details.                            |
|                                                                         |
| You should have received a copy of the GNU General Public License       |
| along with this program.  If not, see <http://www.gnu.org/licenses/>.   |
**************************************************************************/
#pragma once

#ifdef _MSC_VER
#undef ERROR
#endif

#include <QSettings>
#include <QRegularExpression>
#include <QPixmap>
#include <QBuffer>

#include "api/lrc.h"
#include "api/account.h"
#include "api/newaccountmodel.h"
#include "api/newcallmodel.h"
#include "api/behaviorcontroller.h"
#include "api/conversation.h"
#include "api/contactmodel.h"
#include "api/contact.h"
#include "api/datatransfermodel.h"
#include "api/conversationmodel.h"
#include "accountlistmodel.h"

#include "account.h"

#include <settingskey.h>

class LRCInstance {
public:
    static void init() {
        instance();
    };
    static lrc::api::Lrc& getAPI() {
        return *(instance().lrc_);
    };
    static void connectivityChanged() {
        instance().lrc_->connectivityChanged();
    };
    static const lrc::api::NewAccountModel& accountModel() {
        return instance().lrc_->getAccountModel();
    };
    static lrc::api::NewAccountModel* editableAccountModel() {
        return const_cast<lrc::api::NewAccountModel*>(&instance().lrc_->getAccountModel());
    };
    static const lrc::api::BehaviorController& behaviorController() {
        return instance().lrc_->getBehaviorController();
    };
    static const lrc::api::DataTransferModel& dataTransferModel() {
        return instance().lrc_->getDataTransferModel();
    };

    static bool isConnected() {
        return instance().lrc_->isConnected();
    };

    static const lrc::api::account::Info&
    getCurrentAccountInfo() {
        return accountModel().getAccountInfo(instance().selectedAccountId);
    };

    static lrc::api::ConversationModel*
    getCurrentConversationModel() {
        return getCurrentAccountInfo().conversationModel.get();
    };

    static lrc::api::NewCallModel*
    getCurrentCallModel() {
        return getCurrentAccountInfo().callModel.get();
    };

    static const std::string& getSelectedAccountId() {
        return instance().selectedAccountId;
    };

    static void setSelectedAccountId(const std::string& accountId) {
        instance().selectedAccountId = accountId;
        QSettings settings;
        settings.setValue(SettingsKey::selectedAccount, QString::fromStdString(accountId));
    };

    static const std::string& getSelectedConvUid() {
        return instance().selectedConvUid;
    };

    static void setSelectedConvId(const std::string& convUid) {
        instance().selectedConvUid = convUid;
    };

    static const int getCurrentAccountIndex(){
        for (int i = 0; i < accountModel().getAccountList().size(); i++) {
            if (accountModel().getAccountList()[i] == instance().selectedAccountId) {
                return i;
            }
        }
        return -1;
    };

// return 1 if password matches
    static const bool currentAccountPasswordMatch(const std::string& passwd) {
        if (passwd == instance().accountModel().getAccountConfig(instance().getSelectedAccountId()).password) {
            return 1;
        }
        else return 0;
    };

// performs all password error checking (returns 0 if change successfull)
    static const bool changeCurrentPassword(const std::string& accountId,
        const std::string& currentPassword, const std::string& newPassword, const std::string& newPasswordConfirm) {
        if (newPassword == newPasswordConfirm && currentAccountPasswordMatch(currentPassword)) {
            return !instance().accountModel().changeAccountPassword(accountId, currentPassword, newPassword);
        }
        return 1;
    };

    static const QVariant getCurrentAccountData(int role) { // [efficiency improvement]
        return instance().accountListModel_.data(instance().accountListModel_.index(getCurrentAccountIndex()), role);
    };

    // validates the registeredName of user
    // returns 0 if registeredName is valid
    // returns 1 if registeredName does not have form of valid registeredName
    // returns 2 if registeredName is already taken
    static const int currentAccountRegisteredNameValidity(const QString& registeredName) {
        qDebug() << "!!!!!!!! lrcInstance !!!!!!!!!!!!!" << registeredName << NameDirectory::instance().lookupName(nullptr, QString(), registeredName);
        QRegularExpression regExp(" ");
        if (registeredName.size() > 3 && !registeredName.contains(regExp)) {
            if (!NameDirectory::instance().lookupName(nullptr, QString(), registeredName)) {
                return 0;
            }
            else {
                return 2;
            }
        }
        else {
            return 1;
        }
    };

    static void setCurrentAccountEnabledState(bool state) {
        instance().editableAccountModel()->enableAccount(instance().selectedAccountId, state);
    };

    static void setCurrentAccountAvatar(const std::string& avatar) {
        instance().editableAccountModel()->setAvatar(getSelectedAccountId(), avatar);
    };

private:
    std::unique_ptr<lrc::api::Lrc> lrc_;
    AccountListModel accountListModel_;

    static LRCInstance& instance() {
        static LRCInstance instance_;
        return instance_;
    };

    LRCInstance() {
        lrc_ = std::make_unique<lrc::api::Lrc>();
    };

    std::string selectedAccountId;
    std::string selectedConvUid;
};