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

#include "callcenterqmlobjectholder.h"

#include "globalsystemtray.h"
#include "utils.h"

CallCenterQmlObjectHolder::CallCenterQmlObjectHolder(QObject *parent)
    : QObject(parent)
{
}

CallCenterQmlObjectHolder::~CallCenterQmlObjectHolder(){}

void
CallCenterQmlObjectHolder::setCallCenterQmlObjectHolder(QObject* obj)
{
    callCenterQmlObject_ = obj;

    connectCallstatusChangedSignal(LRCInstance::getCurrAccId());

    connect(&LRCInstance::behaviorController(), &BehaviorController::showIncomingCallView, this, &CallCenterQmlObjectHolder::slotShowIncomingCallView);
    connect(&LRCInstance::behaviorController(), &BehaviorController::showCallView, this, &CallCenterQmlObjectHolder::slotShowCallView);
}

void
CallCenterQmlObjectHolder::placeAudioOnlyCall()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getCurrentConversationModel()->placeAudioOnlyCall(convInfo.uid);
    }
}

void
CallCenterQmlObjectHolder::hangUpACall(const QString& accountId, const QString& convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getAccountInfo(accountId).callModel->hangUp(convInfo.callId);
    }
}

void
CallCenterQmlObjectHolder::refuseACall(const QString& accountId, const QString& convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getAccountInfo(accountId).callModel->refuse(convInfo.callId);
    }
}

void
CallCenterQmlObjectHolder::acceptACall(const QString& accountId, const QString& convUid)
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);
    if (!convInfo.uid.isEmpty()) {
        LRCInstance::getAccountInfo(accountId).callModel->accept(convInfo.callId);
    }
}

void
CallCenterQmlObjectHolder::slotShowIncomingCallView(const QString& accountId,
                                                    const conversation::Info& convInfo)
{
    auto callModel = LRCInstance::getCurrentCallModel();

    if (!callModel->hasCall(convInfo.callId)) {
        //auto convModel = LRCInstance::getAccountInfo(accountId).conversationModel.get();
        //auto formattedName = Utils::bestNameForConversation(convInfo, *convModel);
        //if (GlobalSystemTray::instance().getTriggeredAccountId() != accountId) {
        //    GlobalSystemTray::instance().setPossibleOnGoingConversationInfo(convInfo);
        //    Utils::showSystemNotification(
        //        this,
        //        QString(tr("Call incoming from %1 to %2"))
        //        .arg(formattedName)
        //        .arg(Utils::bestNameForAccount(LRCInstance::getAccountInfo(accountId))),
        //        5000,
        //        accountId
        //        );
        //}
        emit showIncomingCallPage(accountId, convInfo.uid);
        return;
    }

    auto convModel = LRCInstance::getCurrentConversationModel();
    //
    auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
    auto bestId = Utils::bestIdForConversation(convInfo, *convModel);
    auto finalBestId = (bestName != bestId) ? bestId : "";

    auto call = callModel->getCall(convInfo.callId);
    auto isCallSelected = LRCInstance::getCurrentConvUid() == convInfo.uid;

    auto itemInCurrentFilter = false;
    if (call.isOutgoing) {
        if (isCallSelected) {
            emit showOutgoingCallPage(accountId, convInfo.uid);
        }
    } else {
        /*if (!QApplication::focusWidget() && GlobalSystemTray::instance().getTriggeredAccountId().isEmpty()) {
            auto formattedName = Utils::bestNameForConversation(convInfo, *convModel);
            Utils::showSystemNotification(this, QString(tr("Call incoming from %1")).arg(formattedName));
        }*/
        auto selectedAccountId = LRCInstance::getCurrentAccountInfo().id;
        auto accountProperties = LRCInstance::accountModel().getAccountConfig(selectedAccountId);
        //if (!isCallSelected)
        //    itemInCurrentFilter = selectSmartlistItem(convInfo.uid);
        if (accountProperties.autoAnswer) {
            //ui->callStackWidget->setCurrentWidget(ui->videoPage);
        } else {
            emit showIncomingCallPage(accountId, convInfo.uid);
        }
        //else if (isCallSelected || !itemInCurrentFilter) {
    }

    //if (!itemInCurrentFilter && !isCallSelected) {
    //    if (ui->smartList->selectionModel())
    //        ui->smartList->selectionModel()->clear();
    //    LRCInstance::setSelectedConvId(convInfo.uid);
    //    showChatView(accountId, convInfo);
    //}
    //else if (ui->messagesWidget->isHidden()) {
    //    ui->messagesWidget->show();
    //}

    emit callStatusChanged(lrc::api::call::to_string(call.status), accountId, convInfo.uid);
    emit callContactImageChanged(Utils::getContactImageString(accountId, convInfo.uid), accountId, convInfo.uid);
    emit setUIBestName(bestName, accountId, convInfo.uid);
    emit setUIBestId(finalBestId, accountId, convInfo.uid);
    //elidedLabel = sencondaryCallLabelFontMetrics.elidedText(finalBestId, Qt::ElideRight, ui->callingBestIdLabel->width());
    //ui->callingBestIdLabel->setText(elidedLabel);
    //ui->sipCallerBestIdLabel->setText(elidedLabel);

    //if (finalBestId.isEmpty())
    //    ui->sipCallerBestIdLabel->setVisible(false);
    //else
    //    ui->sipCallerBestIdLabel->setVisible(true);

    emit updateConversationSmartList();
}

void
CallCenterQmlObjectHolder::slotShowCallView(const QString& accountId,
                                            const lrc::api::conversation::Info& convInfo)
{
    // control visible callwidget buttons
    //emit setCallPanelVisibility(true);

    //ui->callStackWidget->setCurrentWidget(ui->videoPage);
    //hideMiniSpinner();
    auto convModel = LRCInstance::getCurrentConversationModel();

    auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
    auto bestId = Utils::bestIdForConversation(convInfo, *convModel);
    auto finalBestId = (bestName != bestId) ? bestId : "";

    auto call = LRCInstance::getCallInfoForConversation(convInfo, false);
    if (!call) {
        return;
    }

    //overlay_->updateCall(convInfo);

    //audioOnlyAvatar_->setAvatarVisible(call->isAudioOnly);
    if (call->isAudioOnly) {
        emit callStatusChanged(lrc::api::call::to_string(call->status), accountId, convInfo.uid);
        emit callContactImageChanged(Utils::getContactImageString(accountId, convInfo.uid), accountId, convInfo.uid);
        emit setUIBestName(bestName, accountId, convInfo.uid);
        emit setUIBestId(finalBestId, accountId, convInfo.uid);

        emit showAudioCallPage(accountId, convInfo.uid);
        //audioOnlyAvatar_->writeAvatarOverlay(convInfo);
    }

    // preview
    //previewWidget_->setVisible(shouldShowPreview(forceCallOnly));

    // distant
    //ui->distantWidget->setRendererId(call->id);
    //auto isPaused = call->status == lrc::api::call::Status::PAUSED;
    //ui->distantWidget->setVisible(!isPaused)
    
    //ui->videoView->show();
    //ui->videoView->setFocus();
}

void
CallCenterQmlObjectHolder::connectCallstatusChangedSignal(const QString& accountId)
{
    auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
    
    QObject::disconnect(callStatusChangedConnection_);

    callStatusChangedConnection_ = QObject::connect(
        accInfo.callModel.get(),
        &lrc::api::NewCallModel::callStatusChanged,
        [this, accountId](const QString& callId) {
            auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId);
            auto& callModel = accInfo.callModel;
            auto call = callModel->getCall(callId);

            // change status label text
            auto convInfo = LRCInstance::getConversationFromCallId(callId);
            if (!convInfo.uid.isEmpty()) {
                emit callStatusChanged(lrc::api::call::to_string(call.status), accountId, convInfo.uid);
            }

            switch (call.status) {
            case lrc::api::call::Status::INVALID:
            case lrc::api::call::Status::INACTIVE:
            case lrc::api::call::Status::ENDED:
            case lrc::api::call::Status::PEER_BUSY:
            case lrc::api::call::Status::TIMEOUT:
            case lrc::api::call::Status::TERMINATING: {
                LRCInstance::renderer()->removeDistantRenderer(callId);
                if (convInfo.uid.isEmpty()) {
                    break;
                }
                // If it's a conference, change the smartlist index
                // to the next remaining participant.
                bool forceCallOnly{ false };
                if (!convInfo.confId.isEmpty()) {
                    auto callList = LRCInstance::getAPI().getConferenceSubcalls(convInfo.confId);
                    if (callList.empty()) {
                        auto lastConferencee = LRCInstance::instance().popLastConferencee(convInfo.confId);
                        callList.append(lastConferencee);
                        forceCallOnly = true;
                    }
                    for (const auto& callId : callList) {
                        if (!callModel->hasCall(callId)) {
                            continue;
                        }
                        auto otherConv = LRCInstance::getConversationFromCallId(callId);
                        if (!otherConv.uid.isEmpty() && otherConv.uid != convInfo.uid) {
                            LRCInstance::setSelectedConvId(otherConv.uid);
                            //selectSmartlistItem(otherConv.uid);
                            //ui->videoView->updateCall(otherConv.uid, otherConv.accountId, forceCallOnly);
                        }
                    }
                }
                else {
                    //ui->videoView->updateCall();
                    //setCallPanelVisibility(false);
                    //showConversationView();
                    //callTerminating(callId);
                    emit closeCallWindow(accountId, convInfo.uid);
                }

                break;
            }
            case lrc::api::call::Status::CONNECTED:
            case lrc::api::call::Status::IN_PROGRESS: {
                auto convInfo = LRCInstance::getConversationFromCallId(callId, accountId);
                if (!convInfo.uid.isEmpty() && convInfo.uid == LRCInstance::getCurrentConvUid()) {
                    accInfo.conversationModel->selectConversation(convInfo.uid);
                }
                LRCInstance::renderer()->addDistantRenderer(callId);
                //ui->videoView->updateCall();
                break;
            }
            case lrc::api::call::Status::PAUSED:
                //ui->videoView->resetPreview();
                //ui->videoView->updateCall();
                //slotShowCallView(accountId, convInfo);
            default:
                break;
            }

            emit updateConversationSmartList();
        });
}