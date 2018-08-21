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

// for getSelectedAccount()
#include "accountmodel.h"
#include "availableaccountmodel.h""
#include "recentmodel.h"

#include "api/lrc.h"
#include "api/account.h"
#include "api/newaccountmodel.h"
#include "api/newcallmodel.h"
#include "api/behaviorcontroller.h"
#include "api/conversation.h"
#include "api/contactmodel.h"
#include "api/contact.h"
#include "api/datatransfermodel.h"

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
    static const lrc::api::BehaviorController& behaviorController() {
        return instance().lrc_->getBehaviorController();
    };
    static const lrc::api::DataTransferModel& dataTransferModel() {
        return instance().lrc_->getDataTransferModel();
    };
    static bool isConnected() {
        return instance().lrc_->isConnected();
    };

    static Account*
    getSelectedAccount()
    {
        auto idx = AvailableAccountModel::instance().selectionModel()->currentIndex();
        if (idx.isValid()) {
            auto ac = idx.data(static_cast<int>(Ring::Role::Object)).value<Account*>();
            return ac;
        }
        return nullptr;
    }

    static const lrc::api::account::Info&
    getCurrentAccountInfo() {
        auto currentAccountId = getSelectedAccount()->id().toStdString();
        return accountModel().getAccountInfo(currentAccountId);
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
    };

    static const std::string& getSelectedConvUid() {
        return instance().selectedConvUid;
    };

    static void setSelectedConvId(const std::string& convUid) {
        instance().selectedConvUid = convUid;
    };

private:
    std::unique_ptr<lrc::api::Lrc> lrc_;

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