/**************************************************************************
| Copyright (C) 2019 by Savoir-faire Linux                                |
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

#include <QObject>
#include <QSettings>
#include <QRegularExpression>
#include <QPixmap>
#include <QBuffer>

#include "settingskey.h"

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
#include "accountlistmodel.h"

#include "account.h"

using namespace lrc::api;

class LRCInstance : public QObject
{
    Q_OBJECT

public:
    static LRCInstance& instance() {
        static LRCInstance instance_;
        return instance_;
    };
    static void init() {
        instance();
        // TODO: remove when all platforms migrate to avModel
        instance().avModel().deactivateOldVideoModels();
    };
    static Lrc& getAPI() {
        return *(instance().lrc_);
    };
    static void connectivityChanged() {
        instance().lrc_->connectivityChanged();
    };
    static const NewAccountModel& accountModel() {
        return instance().lrc_->getAccountModel();
    };
    static NewAccountModel* editableAccountModel() {
        return const_cast<NewAccountModel*>(&instance().lrc_->getAccountModel());
    };
    static const BehaviorController& behaviorController() {
        return instance().lrc_->getBehaviorController();
    };
    static const DataTransferModel& dataTransferModel() {
        return instance().lrc_->getDataTransferModel();
    };
    static DataTransferModel* editableDataTransferModel() {
        return const_cast<DataTransferModel*>(&instance().lrc_->getDataTransferModel());
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
    getCurrentAccountInfo() {
        try {
            return accountModel().getAccountInfo(getCurrAccId());
        } catch (...) {
            static account::Info invalid = {};
            qWarning() << "getAccountInfo exception";
            return invalid;
        }
    };

    static ConversationModel*
    getCurrentConversationModel() {
        return getCurrentAccountInfo().conversationModel.get();
    };

    static NewCallModel*
    getCurrentCallModel() {
        return getCurrentAccountInfo().callModel.get();
    };

    static const int getAccountNumList() {
        return accountModel().getAccountList().size();
    };

    static const std::string& getCurrAccId() {
        if (instance().selectedAccountId_.empty()) {
            instance().selectedAccountId_ = accountModel().getAccountList().at(0);
        }
        return instance().selectedAccountId_;
    };

    static void setSelectedAccountId(const std::string& accountId) {
        instance().selectedAccountId_ = accountId;
        QSettings settings("jami.net", "Jami");
        settings.setValue(SettingsKey::selectedAccount, QString::fromStdString(accountId));
    };

    static const std::string& getSelectedConvUid() {
        return instance().selectedConvUid_;
    };

    static void setSelectedConvId(const std::string& convUid) {
        instance().selectedConvUid_ = convUid;
    };

    static void reset(bool newInstance = false) {
        if (newInstance) {
            instance().lrc_.reset(new Lrc());
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

    static void setCurrAccAvatar(const QPixmap& avatarPixmap) {
        QByteArray ba;
        QBuffer bu(&ba);
        bu.open(QIODevice::WriteOnly);
        avatarPixmap.save(&bu, "PNG");
        auto str = ba.toBase64().toStdString();
        instance().editableAccountModel()->setAvatar(getCurrAccId(), str);
    };

    static void setCurrAccAvatar(const std::string& avatar) {
        instance().editableAccountModel()->setAvatar(getCurrAccId(), avatar);
    };

    static void setCurrAccDisplayName(const std::string& alias) {
        instance().editableAccountModel()->setAlias(getCurrAccId(), alias);
    };

    static const account::ConfProperties_t& getCurrAccConfig() {
        return instance().getCurrentAccountInfo().confProperties;
    }

signals:
    /// emit once at least one valid account is loaded
    void accountOnBoarded();

private:
    std::unique_ptr<Lrc> lrc_;
    AccountListModel accountListModel_;

    LRCInstance() {
        lrc_ = std::make_unique<Lrc>();
    };

    std::string selectedAccountId_;
    std::string selectedConvUid_;
};
