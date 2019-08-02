/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "videooverlay.h"
#include "ui_videooverlay.h"

#include <QTime>
#include <QMouseEvent>

#include "lrcinstance.h"
#include "contactpicker.h"
#include "utils.h"

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    oneSecondTimer_(new QTimer(this)),
    contactPicker_(new ContactPicker(this))
{
    ui->setupUi(this);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    // transfer/conference related ui
    ui->transferCallButton->setVisible(false);
    ui->transferCallButton->setCheckable(true);

    ui->addToConferenceButton->setVisible(false);
    ui->addToConferenceButton->setCheckable(true);

    contactPicker_->setVisible(false);
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
}

void
VideoOverlay::setupUI(const std::string& callId)
{
    callId_ = callId;

    auto callModel = LRCInstance::getCurrentCallModel();
    if (!LRCInstance::getCurrentCallModel()->hasCall(callId_)) {
        return;
    }

    auto call = callModel->getCall(callId_);

    // timer
    auto timeStr = QString::fromStdString(callModel->getFormattedCallDuration(callId_));
    ui->timerLabel->setText(timeStr);
    disconnect(timerConnection_);
    timerConnection_ = connect(oneSecondTimer_, &QTimer::timeout, this, &VideoOverlay::setTime);
    oneSecondTimer_->start(1000);

    // mute video should not show up on audio only calls
    ui->noVideoButton->setVisible(!call.isAudioOnly);

    // use the callid to get the conversation and it's account type
    auto conv = Utils::getConversationFromCallId(callId_);
    auto accountType = LRCInstance::accountModel().getAccountInfo(conv.accountId).profileInfo.type;

    disconnect(contactPicker_);
    connect(contactPicker_, &ContactPicker::contactWillDoTransfer,
            this, &VideoOverlay::slotWillDoTransfer);
    connect(contactPicker_, &ContactPicker::contactWillJoinConference,
            this, &VideoOverlay::slotWillJoinConference);
    // use the account type to determine the visibility of the transfer/conference ui
    ui->transferCallButton->setVisible(accountType == lrc::api::profile::Type::SIP);
    ui->addToConferenceButton->setVisible(accountType == lrc::api::profile::Type::RING);
    contactPicker_->setType(accountType);

    auto isAudioMuted = call.audioMuted && (call.status != lrc::api::call::Status::PAUSED);
    auto isVideoMuted = call.videoMuted && (call.status != lrc::api::call::Status::PAUSED) && (!call.isAudioOnly);
    auto isRecording = callModel->isRecording(callId);
    auto isHolding = call.status == lrc::api::call::Status::PAUSED;

    // Block the signals of buttons while setting states
    Utils::whileBlocking(ui->noMicButton)->setChecked(isAudioMuted);
    Utils::whileBlocking(ui->noVideoButton)->setChecked(isVideoMuted);
    Utils::whileBlocking(ui->recButton)->setChecked(isRecording);
    Utils::whileBlocking(ui->holdButton)->setChecked(isHolding);
    Utils::whileBlocking(ui->onHoldLabel)->setVisible(isHolding);
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setTime()
{
    if (callId_.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId_)) {
        return;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId_);
    if (callInfo.status == lrc::api::call::Status::IN_PROGRESS ||
        callInfo.status == lrc::api::call::Status::PAUSED) {
        int numSeconds = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - callInfo.startTime).count();
        QTime t(0, 0, numSeconds);
        ui->timerLabel->setText(t.toString(numSeconds > 3600 ? "hh:mm:ss" : "mm:ss"));
    }
}

bool
VideoOverlay::shouldShowOverlay()
{
    if (!LRCInstance::getCurrentCallModel()->hasCall(callId_)) {
        return false;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId_);
    return  ui->bottomButtons->underMouse() ||
            ui->topInfoBar->underMouse() ||
            (callInfo.status == lrc::api::call::Status::PAUSED) ||
            contactPicker_->isActiveWindow();
}

void
VideoOverlay::simulateShowChatview(bool checked)
{
    ui->chatButton->setChecked(checked);
}

bool
VideoOverlay::getShowChatView()
{
    return ui->chatButton->isChecked();
}

void
VideoOverlay::on_hangupButton_clicked()
{
    auto convInfo = Utils::getSelectedConversation();
    if (!convInfo.uid.empty()) {
        auto& callId = convInfo.callId;
        auto callModel = LRCInstance::getCurrentCallModel();
        if (callModel->hasCall(callId)) {
            callModel->hangUp(callId);
        }
    }
    ui->chatButton->setChecked(false);
}

void
VideoOverlay::on_chatButton_toggled(bool checked)
{
    emit setChatVisibility(checked);
}

void
VideoOverlay::on_addToConferenceButton_toggled(bool checked)
{
    if (callId_.empty() || !checked) {
        return;
    }
    QPoint globalPos_button = mapToGlobal(ui->addToConferenceButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    contactPicker_->move(globalPos_button.x(), globalPos_bottomButtons.y() - contactPicker_->height());

    // receive the signal that ensure the button checked status is correct and contactpicker
    // is properly hidden
    Utils::oneShotConnect(contactPicker_, &ContactPicker::willClose,
        [this](QMouseEvent* event) {
            contactPicker_->hide();
            // check if current mouse position is on button
            auto relativeClickPos = ui->addToConferenceButton->mapFromGlobal(event->globalPos());
            if (!ui->addToConferenceButton->rect().contains(relativeClickPos)) {
                ui->addToConferenceButton->setChecked(false);
            }
        });

    // for esc key, receive reject signal
    Utils::oneShotConnect(contactPicker_, &QDialog::rejected,
        [this] {
            ui->addToConferenceButton->setChecked(false);
        });

    contactPicker_->show();
}

void
VideoOverlay::on_holdButton_clicked()
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->togglePause(callId_);
        auto onHold = callModel->getCall(callId_).status == lrc::api::call::Status::PAUSED;
        ui->holdButton->setChecked(!onHold);
        ui->onHoldLabel->setVisible(!onHold);
    }
}

void
VideoOverlay::on_noMicButton_toggled(bool checked)
{
    Q_UNUSED(checked);
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::AUDIO);
    }
}

void
VideoOverlay::on_noVideoButton_toggled(bool checked)
{
    Q_UNUSED(checked);
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::VIDEO);
    }
}

void
VideoOverlay::on_recButton_clicked()
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleAudioRecord(callId_);
    }
}

void
VideoOverlay::on_transferCallButton_toggled(bool checked)
{
    if (callId_.empty() || !checked) {
        return;
    }

    QPoint globalPos_button = mapToGlobal(ui->transferCallButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    contactPicker_->move(globalPos_button.x(), globalPos_bottomButtons.y() - contactPicker_->height());

    // receive the signal that ensure the button checked status is correct and contactpicker
    // is properly hidden
    Utils::oneShotConnect(contactPicker_, &ContactPicker::willClose,
        [this](QMouseEvent *event) {
            contactPicker_->hide();
            // check if current mouse position is on button
            auto relativeClickPos = ui->transferCallButton->mapFromGlobal(event->globalPos());
            if (!ui->transferCallButton->rect().contains(relativeClickPos)) {
                ui->transferCallButton->setChecked(false);
            }
        });

    // for esc key, receive reject signal
    Utils::oneShotConnect(contactPicker_, &QDialog::rejected,
    [this] {
        ui->transferCallButton->setChecked(false);
    });

    contactPicker_->show();
}

void
VideoOverlay::slotWillDoTransfer(const std::string& callId, const std::string& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->transferCallButton->setChecked(false);
    std::string destCallId;

    try {
        //check if the call exist - (check non-finished calls)
        auto callInfo = callModel->getCallFromURI(contactUri, true);
        destCallId = callInfo.id;
    } catch(std::exception& e) {
        qDebug().noquote() << e.what();
        destCallId = "";
    }
    // if no second call -> blind transfer
    // if there is a second call -> attended transfer
    if (destCallId.size() == 0) {
        callModel->transfer(callId, "sip:" + contactUri);
        callModel->hangUp(callId);
    }else{
        callModel->transferToCall(callId, destCallId);
        callModel->hangUp(callId);
        callModel->hangUp(destCallId);
    }
}

void
VideoOverlay::slotWillJoinConference(const std::string& callId, const std::string& contactUri)
{
    qDebug() << "slotWillJoinConference";
    auto callModel = LRCInstance::getCurrentCallModel();
    if (!callModel->hasCall(callId)) {

    }
    auto thisCallIsAudioOnly = callModel->getCall(callId).isAudioOnly;

    // generate new call with peer
    auto conferenceeCallId = callModel->createCall(contactUri, thisCallIsAudioOnly);
    pendingConferencees_.insert(QString::fromStdString(conferenceeCallId),
        QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
            [this, conferenceeCallId, callId](const std::string& callId) {
                if (callId != conferenceeCallId)
                    return;
                using namespace lrc::api::call;
                auto call = LRCInstance::getCurrentCallModel()->getCall(callId);
                switch (call.status) {
                case Status::IN_PROGRESS:
                {
                    qDebug() << "adding to conference callid=" << QString::fromStdString(callId);
                    auto it = pendingConferencees_.find(QString::fromStdString(conferenceeCallId));
                    if (it != pendingConferencees_.end()) {
                        QObject::disconnect(it.value());
                        pendingConferencees_.erase(it);
                    }
                    LRCInstance::getCurrentCallModel()->joinCalls(callId, conferenceeCallId);
                    return;
                }
                default:
                    qDebug() << "failed to add to conference callid=" << QString::fromStdString(callId);
                    break;
                }
            })
    );
}

void
VideoOverlay::setCurrentSelectedCalleeDisplayName(const QString& CalleeDisplayName)
{
    contactPicker_->setCurrentCalleeDisplayName(CalleeDisplayName);
}
