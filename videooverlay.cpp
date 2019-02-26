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

#include "contactpicker.h"

// LRC
#include "callmodel.h"
#include "person.h"
#include "account.h"

#include "lrcinstance.h"
#include "utils.h"

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    oneSecondTimer_(new QTimer(this))
{
    ui->setupUi(this);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);
    ui->noMicButton->setCheckable(true);
    ui->onHoldLabel->setVisible(false);
    ui->recButton->setVisible(false);

}

VideoOverlay::~VideoOverlay()
{
    delete ui;
}

void
VideoOverlay::callStarted(const std::string& callId)
{
    ui->timerLabel->setText("00:00");
    callId_ = callId;
    connect(oneSecondTimer_, &QTimer::timeout, this, &VideoOverlay::setTime);
    oneSecondTimer_->start(1000);
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setTime()
{
    if (callId_.empty()) {
        return;
    }
    try {
        auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId_);
        if (callInfo.status == lrc::api::call::Status::IN_PROGRESS ||
            callInfo.status == lrc::api::call::Status::PAUSED) {
            int numSeconds = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - callInfo.startTime).count();
            QTime t(0, 0, numSeconds);
            ui->timerLabel->setText(t.toString(numSeconds > 3600 ? "hh:mm:ss" : "mm:ss"));
        }
    } catch (...) { }
}

void
VideoOverlay::setVideoMuteVisibility(bool visible)
{
    ui->noVideoButton->setVisible(visible);
}

bool
VideoOverlay::shouldShowOverlay()
{
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId_);
    auto callPaused = callInfo.status == lrc::api::call::Status::PAUSED;
    return ui->bottomButtons->underMouse() || ui->topInfoBar->underMouse() || callPaused;
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
VideoOverlay::on_addToConferenceButton_clicked()
{
    if (callId_.empty()) {
        return;
    }
    QPoint globalPos_button = mapToGlobal(ui->addToConferenceButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());
    ContactPicker conferenceePicker;
    QObject::connect(&conferenceePicker, &ContactPicker::contactWillJoinConference,
        [this](const std::string& callId1, const std::string& contactUri) {
            auto callModel = LRCInstance::getCurrentCallModel();
            auto thisCallIsAudioOnly = callModel->getCall(callId1).isAudioOnly;

            // generate new call with peer
            auto conferenceeCallId = callModel->createCall(contactUri, thisCallIsAudioOnly);
            pendingConferencees_.insert(QString::fromStdString(conferenceeCallId),
                QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
                    [this, conferenceeCallId, callId1](const std::string& callId) {
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
                            LRCInstance::getCurrentCallModel()->joinCalls(callId1, conferenceeCallId);
                            return;
                        }
                        default:
                            qDebug() << "failed to add to conference callid=" << QString::fromStdString(callId);
                            break;
                        }
                    })
            );
        });
    conferenceePicker.move(globalPos_button.x(), globalPos_bottomButtons.y() - conferenceePicker.height());
    conferenceePicker.exec();
}

void
VideoOverlay::on_holdButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->togglePause(callId);
        auto onHold = callModel->getCall(callId).status == lrc::api::call::Status::PAUSED;
        ui->holdButton->setChecked(!onHold);
        ui->onHoldLabel->setVisible(!onHold);
    }
}

void
VideoOverlay::on_noMicButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::AUDIO);
        ui->noMicButton->setChecked(callModel->getCall(callId).audioMuted);
    }
}

void
VideoOverlay::on_noVideoButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::VIDEO);
        ui->noVideoButton->setChecked(callModel->getCall(callId).videoMuted);
    }
}

void
VideoOverlay::on_recButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleAudioRecord(callId);
    }
}
