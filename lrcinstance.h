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

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QRegularExpression>
#include <QPixmap>
#include <QBuffer>

#include "settingskey.h"
#include "accountlistmodel.h"
#include "utils.h"

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

#include <memory>

class FrameWrapper : public QObject
{

    Q_OBJECT

public:
    FrameWrapper(bool isPreview);
    ~FrameWrapper();

    void connectPreviewRendering();
    lrc::api::video::Renderer* getPreviewRenderer() { return previewRenderer_;  }
    lrc::api::video::Frame getPreviewFrame() { return previewFrame_; }

signals:
    void previewRenderReady();
    void previewRenderStopped();

private slots:
    void slotPreviewStarted(const std::string& id = {});
    void slotPreviewUpdated(const std::string& id = {});
    void slotPreviewStoped(const std::string& id = {});

private:
    bool isPreview_;
    lrc::api::video::Renderer* previewRenderer_;
    lrc::api::video::Frame previewFrame_;

    QMutex mutex_;

    struct frameWrapperConnections {
        QMetaObject::Connection started, stopped, updated;
    } frameWrapperConnections_;

    void renderFrame(const std::string& id);
};

class RenderDistributer : public QObject
{

    Q_OBJECT

public:
    RenderDistributer();
    ~RenderDistributer();
    lrc::api::video::Renderer* getPreviewRenderer() { return previewFrameWrapper_->getPreviewRenderer(); }
    lrc::api::video::Frame getPreviewFrame() { return previewFrameWrapper_->getPreviewFrame(); }
    void connectPreviewRendering() { previewFrameWrapper_->connectPreviewRendering(); }

signals:
    void previewRenderReady();
    void previewRenderStopped();

private:
    // one preview to rule them all
    std::unique_ptr<FrameWrapper> previewFrameWrapper_;
    // distant for each call/conf/conversation
    //std::map<std::string, std::unique_ptr<FrameWrapper>> distantFrames_;
};

using namespace lrc::api;

using migrateCallback = std::function<void()>;

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
    static RenderDistributer* getRenderDistributer() {
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

    static const std::string& getSelectedConvUid() {
        return instance().selectedConvUid_;
    };

    static void setSelectedConvId(const std::string& convUid = {}) {
        instance().selectedConvUid_ = convUid;
    };

    static bool getIfCurrentSelectedCallIsAudioOnly() {
        auto isAudioOnly = false;
        auto convInfo = Utils::getSelectedConversation();
        if (!convInfo.uid.empty()) {
            isAudioOnly = LRCInstance::getCurrentCallModel()->getCall(convInfo.callId).isAudioOnly;
        }
        return isAudioOnly;
    };

    static void reset(bool newInstance = false) {
        if (newInstance) {
            instance().lrc_.reset(new Lrc());
            instance().renderer_.reset(new RenderDistributer());
        } else {
            instance().lrc_.reset();
            instance().renderer_.reset();
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

    static void setCurrAccDisplayName(const std::string& alias) {
        accountModel().setAlias(getCurrAccId(), alias);
    };

    static const account::ConfProperties_t& getCurrAccConfig() {
        return instance().getCurrentAccountInfo().confProperties;
    }

    static void subscribeToDebugReceived() {
        instance().lrc_->subscribeToDebugReceived();
    }

signals:
    void accountListChanged();

private:
    std::unique_ptr<Lrc> lrc_;
    AccountListModel accountListModel_;

    LRCInstance(migrateCallback willMigrateCb = {},
                migrateCallback didMigrateCb = {}) {
        lrc_ = std::make_unique<Lrc>(willMigrateCb, didMigrateCb);
        renderer_ = std::make_unique<RenderDistributer>();
    };

    std::string selectedAccountId_;
    std::string selectedConvUid_;

    std::unique_ptr<RenderDistributer> renderer_;
};
