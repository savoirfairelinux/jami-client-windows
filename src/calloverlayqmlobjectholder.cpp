/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "calloverlayqmlobjectholder.h"

#include "utils.h"

CallOverlayQmlObjectHolder::CallOverlayQmlObjectHolder(QObject *parent)
    : QObject(parent)
    , oneSecondTimer_(new QTimer(this))
{}

CallOverlayQmlObjectHolder::~CallOverlayQmlObjectHolder() {}

void
CallOverlayQmlObjectHolder::setCallOverlayQmlObjectHolder(QObject *obj)
{
    callOverlayQmlObject_ = obj;
}

void
CallOverlayQmlObjectHolder::updateCallOverlay(const QString &accountId, const QString &convUid)
{
    // ?
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid, accountId);

    accountId_ = convInfo.accountId;
    convUid_ = convInfo.uid;

    setTime(convInfo.accountId, convInfo.uid);
    QObject::disconnect(oneSecondTimer_);
    QObject::connect(oneSecondTimer_, &QTimer::timeout, [this] { setTime(accountId_, convUid_); });
    oneSecondTimer_->start(20);

    auto &accInfo = LRCInstance::accountModel().getAccountInfo(convInfo.accountId);

    auto &convModel = accInfo.conversationModel;

    auto call = LRCInstance::getCallInfoForConversation(convInfo);
    if (!call) {
        return;
    }

    bool isPaused = call->status == lrc::api::call::Status::PAUSED;
    bool isAudioOnly = call->isAudioOnly && !isPaused;
    bool isAudioMuted = call->audioMuted && (call->status != lrc::api::call::Status::PAUSED);
    bool isVideoMuted = call->videoMuted && !isPaused && !call->isAudioOnly;
    bool isRecording = accInfo.callModel->isRecording(convInfo.callId);

    emit buttonStatusChanged(isPaused, isAudioOnly, isAudioMuted, isVideoMuted, isRecording);
    emit updateBestName(
        Utils::bestNameForConversation(convInfo, *LRCInstance::getCurrentConversationModel()));
    /*//Set irrelevant buttons invisible
    ui->noVideoButton->setVisible(!isAudioOnly);

    // Block the signals of buttons
    ui->noMicButton->setOverlayButtonChecked(isAudioMuted);
    ui->noVideoButton->setOverlayButtonChecked(isVideoMuted);
    ui->recButton->setOverlayButtonChecked(isRecording);
    ui->holdButton->setOverlayButtonChecked(isPaused);
    ui->onHoldLabel->setVisible(isPaused);

    bool isSIP = accInfo.profileInfo.type == lrc::api::profile::Type::SIP;
    ui->addToConferenceButton->setVisible(!isSIP);
    ui->transferCallButton->setVisible(isSIP);
    ui->sipInputPanelButton->setVisible(isSIP);

    // only show the hold/pause button for non-conference calls
    ui->holdButton->setVisible(convInfo.confId.isEmpty());*/
}

void
CallOverlayQmlObjectHolder::hangUpThisCall()
{
    auto convInfo = LRCInstance::getConversationFromConvUid(convUid_, accountId_);
    if (!convInfo.uid.isEmpty()) {
        auto callModel = LRCInstance::getAccountInfo(accountId_).callModel.get();
        if (callModel->hasCall(convInfo.callId)) {
            // Store the last remaining participant of the conference
            // so we can switch the smartlist index after termination.
            if (!convInfo.confId.isEmpty()) {
                auto callList = LRCInstance::getAPI().getConferenceSubcalls(convInfo.confId);
                if (callList.size() == 2) {
                    for (const auto &cId : callList) {
                        if (cId != convInfo.callId) {
                            LRCInstance::instance().pushLastConferencee(convInfo.confId, cId);
                        }
                    }
                }
            }
            callModel->hangUp(convInfo.callId);
        }
    }
}

void
CallOverlayQmlObjectHolder::holdThisCallToggle()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->togglePause(callId);
    }
    emit showOnHoldLabel(true);
}

void
CallOverlayQmlObjectHolder::muteThisCallToggle()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::AUDIO);
    }
}

void
CallOverlayQmlObjectHolder::recordThisCallToggle()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleAudioRecord(callId);
    }
}

void
CallOverlayQmlObjectHolder::setTime(const QString &accountId, const QString &convUid)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid, accountId);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    if (callInfo.status == lrc::api::call::Status::IN_PROGRESS
        || callInfo.status == lrc::api::call::Status::PAUSED) {
        auto timeString = LRCInstance::getCurrentCallModel()->getFormattedCallDuration(callId);
        emit updateTimeText(timeString);
    }
}