/**************************************************************************
| Copyright (C) 2015-2018 by Savoir-faire Linux                           |
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          |
| Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      |
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
| along with this program.  If not, see <https://www.gnu.org/licenses/>.  |
**************************************************************************/
#pragma once

#ifdef _MSC_VER
#undef ERROR
#endif

#include <QSettings>
#include <QRegularExpression>
#include <QPixmap>
#include <QBuffer>
#include <QObject>

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
//#include "instancemanager.cpp"

#include <settingskey.h>

class LRCInstance : public QObject {
    Q_OBJECT
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
    static lrc::api::DataTransferModel* editableDataTransferModel() {
        return const_cast<lrc::api::DataTransferModel*>(&instance().lrc_->getDataTransferModel());
    };
    static bool isConnected() {
        return instance().lrc_->isConnected();
    };

    static const lrc::api::account::Info&
    getCurrentAccountInfo() {
        return accountModel().getAccountInfo(getCurrAccId());
    };

    static lrc::api::ConversationModel*
    getCurrentConversationModel() {
        return getCurrentAccountInfo().conversationModel.get();
    };

    static lrc::api::NewCallModel*
    getCurrentCallModel() {
        return getCurrentAccountInfo().callModel.get();
    };

    static const int getAccountNumList() {
        return accountModel().getAccountList().size();
    };

    static const std::string& getCurrAccId() {
        if (instance().selectedAccountId.empty()) {
            instance().selectedAccountId = accountModel().getAccountList().at(0);
        }
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

    static void reset(bool newInstance = false) {
        if (newInstance) {
            instance().lrc_.reset(new lrc::api::Lrc());
        } else {
            instance().lrc_.reset();
        }
    };

    static const int getCurrentAccountIndex(){
        for (int i = 0; i < accountModel().getAccountList().size(); i++) {
            if (accountModel().getAccountList()[i] == getCurrAccId()) {
                return i;
            }
        }
        return -1;
    };

    static const QPixmap getCurrAccPixmap() {
        return instance().accountListModel_.data(instance().accountListModel_
            .index(getCurrentAccountIndex()), AccountListModel::Role::Picture).value<QPixmap>();
    };

    static void setCurrAccAvatar(const std::string& avatar) {
        instance().editableAccountModel()->setAvatar(getCurrAccId(), avatar);
    };

    static void setCurrAccDisplayName(const std::string& alias) {
        instance().editableAccountModel()->setAlias(getCurrAccId(), alias);
    };

    static const lrc::api::account::ConfProperties_t& getCurrAccConfig() {
        return instance().getCurrentAccountInfo().confProperties;
    }

    void
    connectAccount(const std::string& accId)
    {
        auto callModel = lrc_->getAccountModel().getAccountInfo(accId).callModel.get();
        QObject::connect(callModel, &lrc::api::NewCallModel::callStarted,
            [this, accId](const std::string& callId) {
                auto callIds = callIdList_[accId];
                callIds.emplace_back(callId);
            });
        QObject::connect(callModel, &lrc::api::NewCallModel::callEnded,
            [this, accId](const std::string& callId) {
                try {
                    auto callIds = callIdList_.at(accId);
                    auto toErase = std::find(callIds.begin(), callIds.end(), callId);
                    if (toErase != callIds.end()) {
                        callIds.erase(toErase);
                    }
                } catch (...) {}
            });
    }

    void
    disconnectAccount(const std::string& accId)
    {
        (void)accId;
    }

private:
    std::unique_ptr<lrc::api::Lrc> lrc_;
    AccountListModel accountListModel_;

    static LRCInstance& instance() {
        static LRCInstance instance_;
        return instance_;
    };

    LRCInstance() {
        lrc_ = std::make_unique<lrc::api::Lrc>();


        for (auto accountId : lrc_->getAccountModel().getAccountList()) {
            connectAccount(accountId);
        }

        QObject::connect(&lrc_->getAccountModel(), &lrc::api::NewAccountModel::accountAdded,
            [this](const std::string& accountId) {
                connectAccount(accountId);
            });

        QObject::connect(&lrc_->getAccountModel(), &lrc::api::NewAccountModel::accountRemoved,
            [this](const std::string& accountId) {
                disconnectAccount(accountId);
            });
    };

    std::string selectedAccountId;
    std::string selectedConvUid;

    std::map<std::string, std::vector<std::string>> callIdList_;
    std::map<std::string, std::pair<QMetaObject::Connection, QMetaObject::Connection>> conList_;
};
