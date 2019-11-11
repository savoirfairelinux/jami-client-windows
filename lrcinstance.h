/**************************************************************************
| Copyright (C) 2019 by Savoir-faire Linux                                |
| Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          |
| Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      |
| Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              |
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

#include "settingskey.h"
#include "accountlistmodel.h"
#include "utils.h"
#include "rendermanager.h"
#include "networkmanager.h"

#include "api/lrc.h"
#include "api/account.h"
#include "api/newaccountmodel.h"
#include "api/newcallmodel.h"
#include "api/newdevicemodel.h"
#include "api/newcodecmodel.h"
#include "api/behaviorcontroller.h"
#include "api/avmodel.h"
#include "api/conversation.h"
#include "api/contactmodel.h"
#include "api/contact.h"
#include "api/datatransfermodel.h"
#include "api/conversationmodel.h"
#include "api/peerdiscoverymodel.h"

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QRegularExpression>
#include <QPixmap>
#include <QBuffer>
#include <QtConcurrent/QtConcurrent>

#include <memory>

using namespace lrc::api;

using migrateCallback = std::function<void()>;
using getConvPredicate = std::function<bool(const conversation::Info& conv)>;

class LRCInstance : public QObject
{
    Q_OBJECT

public:
    static LRCInstance& instance(migrateCallback willMigrate = {},
                                 migrateCallback didMigrate = {}) {
        static LRCInstance instance_(willMigrate, didMigrate);
        return instance_;
    };
    static void init(migrateCallback willMigrate = {},
                     migrateCallback didMigrate = {}) {
        instance(willMigrate, didMigrate);
    };
    static Lrc& getAPI() {
        return *(instance().lrc_);
    };
    static RenderManager* renderer() {
        return instance().renderer_.get();
    }
    static void connectivityChanged() {
        instance().lrc_->connectivityChanged();
    };
    static NewAccountModel& accountModel() {
        return instance().lrc_->getAccountModel();
    };
    static BehaviorController& behaviorController() {
        return instance().lrc_->getBehaviorController();
    };
    static DataTransferModel& dataTransferModel() {
        return instance().lrc_->getDataTransferModel();
    };
    static AVModel& avModel() {
        return instance().lrc_->getAVModel();
    };
    static bool isConnected() {
        return instance().lrc_->isConnected();
    };
    static std::vector<std::string> getActiveCalls() {
        return instance().lrc_->activeCalls();
    };
    static const account::Info&
    getAccountInfo(const std::string& accountId) {
        return accountModel().getAccountInfo(accountId);
    };
    static const account::Info&
    getCurrentAccountInfo() {
        return getAccountInfo(getCurrAccId());
    };
    static bool hasVideoCall() {
        auto activeCalls = instance().lrc_->activeCalls();
        auto accountList = accountModel().getAccountList();
        bool result = false;
        for (const auto& callId : activeCalls) {
            for (const auto& accountId : accountList) {
                auto& accountInfo = accountModel().getAccountInfo(accountId);
                if (accountInfo.callModel->hasCall(callId)) {
                    auto call = accountInfo.callModel->getCall(callId);
                    result |= !(call.isAudioOnly || call.videoMuted);
                }
            }
        }
        return result;
    };
    static std::string
    getCallIdForConversationUid(const std::string& convUid, const std::string& accountId)
    {
        auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
        if (convInfo.uid.empty()) {
            return {};
        }
        return convInfo.confId.empty() ? convInfo.callId : convInfo.confId;
    }
    static const call::Info*
    getCallInfo(const std::string& callId, const std::string& accountId) {
        try {
            auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
            if (!accInfo.callModel->hasCall(callId)) {
                return nullptr;
            }
            return &accInfo.callModel->getCall(callId);
        } catch (...) {
            return nullptr;
        }
    }
    static const call::Info*
    getCallInfoForConversation(const conversation::Info& convInfo) {
        try {
            auto accountId = convInfo.accountId;
            auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
            auto callId = convInfo.confId.empty() ? convInfo.callId : convInfo.confId;
            if (!accInfo.callModel->hasCall(callId)) {
                return nullptr;
            }
            return &accInfo.callModel->getCall(callId);
        } catch(...) {
            return nullptr;
        }
    }
    static const conversation::Info&
    getConversation(const std::string& accountId,
                    getConvPredicate pred = {},
                    bool filtered=false) {
        using namespace lrc::api;
        static conversation::Info invalid = {};
        try {
            auto& accInfo = LRCInstance::getAccountInfo(accountId);
            auto& convModel = accInfo.conversationModel;
            if (filtered) {
                auto& convs = convModel->allFilteredConversations();
                auto conv = std::find_if(convs.begin(), convs.end(), pred);
                if (conv != convs.end()) {
                    return *conv;
                }
            } else {
                for (int i = Utils::toUnderlyingValue(profile::Type::RING);
                     i <= Utils::toUnderlyingValue(profile::Type::TEMPORARY);
                     ++i) {
                    auto filter = Utils::toEnum<profile::Type>(i);
                    auto& convs = convModel->getFilteredConversations(filter);
                    auto conv = std::find_if(convs.begin(), convs.end(), pred);
                    if (conv != convs.end()) {
                        return *conv;
                    }
                }
            }
        } catch(...) {}
        return invalid;
    }
    static const conversation::Info&
    getConversationFromCallId(const std::string& callId,
                              const std::string& accountId = {},
                              bool filtered = false) {
        return getConversation(!accountId.empty() ? accountId : getCurrAccId(),
            [&](const conversation::Info& conv) -> bool {
                return callId == conv.callId;
            }, filtered);
    }
    static const conversation::Info&
    getConversationFromConvUid(const std::string& convUid,
                               const std::string& accountId = {},
                               bool filtered = false) {
        return getConversation(!accountId.empty() ? accountId : getCurrAccId(),
            [&](const conversation::Info& conv) -> bool {
                return convUid == conv.uid;
            }, filtered);
    }
    static const conversation::Info&
    getConversationFromPeerUri(const std::string& peerUri,
                               const std::string& accountId = {},
                               bool filtered = false) {
        return getConversation(!accountId.empty() ? accountId : getCurrAccId(),
            [&](const conversation::Info& conv) -> bool {
                return peerUri == conv.participants[0];
            }, filtered);
    }
    static const conversation::Info&
    getCurrentConversation() {
        return getConversationFromConvUid(getCurrentConvUid());
    }

    static ConversationModel*
    getCurrentConversationModel() {
        return getCurrentAccountInfo().conversationModel.get();
    };

    static NewCallModel*
    getCurrentCallModel() {
        return getCurrentAccountInfo().callModel.get();
    };

    static const std::string& getCurrAccId() {
        auto accountList = accountModel().getAccountList();
        if (instance().selectedAccountId_.empty() && accountList.size()) {
            instance().selectedAccountId_ = accountList.at(0);
        }
        return instance().selectedAccountId_;
    };

    static void setSelectedAccountId(const std::string& accountId = {}) {
        instance().selectedAccountId_ = accountId;
        QSettings settings("jami.net", "Jami");
        settings.setValue(SettingsKey::selectedAccount, QString::fromStdString(accountId));
    };

    static const std::string& getCurrentConvUid() {
        return instance().selectedConvUid_;
    };

    static void setSelectedConvId(const std::string& convUid = {}) {
        instance().selectedConvUid_ = convUid;
    };

    static void reset(bool newInstance = false) {
        if (newInstance) {
            instance().renderer_.reset(new RenderManager(avModel()));
            instance().lrc_.reset(new Lrc());
        } else {
            instance().renderer_.reset();
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

    static void setCurrAccAvatar(const QPixmap& avatarPixmap) {
        QByteArray ba;
        QBuffer bu(&ba);
        bu.open(QIODevice::WriteOnly);
        avatarPixmap.save(&bu, "PNG");
        auto str = ba.toBase64().toStdString();
        accountModel().setAvatar(getCurrAccId(), str);
    };

    static void setCurrAccAvatar(const std::string& avatar) {
        accountModel().setAvatar(getCurrAccId(), avatar);
    };

    static void setCurrAccDisplayName(const std::string& displayName) {
        auto accountId = LRCInstance::getCurrAccId();
        accountModel().setAlias(accountId, displayName);
        // force save to .yml
        auto confProps = LRCInstance::accountModel().getAccountConfig(accountId);
        LRCInstance::accountModel().setAccountConfig(accountId, confProps);
    };

    static const account::ConfProperties_t& getCurrAccConfig() {
        return instance().getCurrentAccountInfo().confProperties;
    }

    static void subscribeToDebugReceived() {
        instance().lrc_->subscribeToDebugReceived();
    }

    static NetWorkManager* getNetworkManager() {
        return instance().netWorkManager_.get();
    }

    static void startAudioMeter(bool async)
    {
        auto f = [] {
            if (!LRCInstance::getActiveCalls().size()) {
                LRCInstance::avModel().startAudioDevice();
            }
            LRCInstance::avModel().setAudioMeterState(true);
        };
        if (async) {
            QtConcurrent::run(f);
        } else {
            f();
        }
    }

    static void stopAudioMeter(bool async)
    {
        auto f = [] {
            if (!LRCInstance::getActiveCalls().size()) {
                LRCInstance::avModel().stopAudioDevice();
            }
            LRCInstance::avModel().setAudioMeterState(false);
        };
        if (async) {
            QtConcurrent::run(f);
        } else {
            f();
        }
    }
signals:
    void accountListChanged();

private:
    LRCInstance(migrateCallback willMigrateCb = {},
                migrateCallback didMigrateCb = {}) {
        lrc_ = std::make_unique<Lrc>(willMigrateCb, didMigrateCb);
        renderer_ = std::make_unique<RenderManager>(lrc_->getAVModel());
        netWorkManager_ = std::make_unique<NetWorkManager>();
    };

    std::unique_ptr<Lrc> lrc_;
    std::unique_ptr<RenderManager> renderer_;
    std::unique_ptr<NetWorkManager> netWorkManager_;
    AccountListModel accountListModel_;
    std::string selectedAccountId_;
    std::string selectedConvUid_;
};
