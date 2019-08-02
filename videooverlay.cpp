/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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

#include "lrcinstance.h"
#include "utils.h"

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    oneSecondTimer_(new QTimer(this))
{
    ui->setupUi(this);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    setAttribute(Qt::WA_NoSystemBackground);

    noMicOffIcon = new QIcon(":/images/icons/ic_mic_white_24dp.png");
    noMicOnIcon = new QIcon(":/images/icons/ic_mic_off_white_24dp.png");
    noVideoOffIcon = new QIcon(":/images/icons/ic_videocam_white.png");
    noVideoOnIcon = new QIcon(":/images/icons/ic_videocam_off_white_24dp.png");

    isMicDisabled[false] = *noMicOffIcon;
    isMicDisabled[true] = *noMicOnIcon;
    isVideoDisabled[false] = *noVideoOffIcon;
    isVideoDisabled[true] = *noVideoOnIcon;



    ui->noMicButton->setCheckable(false);
    ui->noMicButton->setIcon(isMicDisabled[false]);

    ui->noVideoButton->setCheckable(false);
    ui->noVideoButton->setIcon(isVideoDisabled[false]);

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
VideoOverlay::on_holdButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        auto onHold = callModel->getCall(callId).status == lrc::api::call::Status::PAUSED;
        ui->holdButton->setChecked(!onHold);
        ui->onHoldLabel->setVisible(!onHold);
        callModel->togglePause(callId);
    }
}

void
VideoOverlay::on_noMicButton_clicked()
{
    bool btn_status = false;
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::AUDIO);
        btn_status = callModel->getCall(callId).audioMuted;
        ui->noMicButton->setIcon(isMicDisabled[btn_status]);
    }
    if (!btn_status) {
        ui->noMicButton->setStyleSheet("QPushButton {background-color: rgba(0, 0, 0, 140);    border-radius: 18px;    border: solid 1px;}"
                                       "QPushButton:hover{background-color: rgba(0, 192, 213, 0.6);    }    ");
    } else {
        ui->noMicButton->setStyleSheet("QPushButton{    background-color: rgba(0, 192, 213, 0.6);}");
    }
}

void
VideoOverlay::on_noVideoButton_clicked()
{
    bool btn_status = false;
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::VIDEO);
        btn_status = callModel->getCall(callId).videoMuted;
        ui->noVideoButton->setIcon(isVideoDisabled[btn_status]);
    }
    if (!btn_status) {
        ui->noVideoButton->setStyleSheet("QPushButton {background-color: rgba(0, 0, 0, 140);    border-radius: 18px;    border: solid 1px;}"
                                         "QPushButton:hover{background-color: rgba(0, 192, 213, 0.6);    }    ");
    } else {
        ui->noVideoButton->setStyleSheet("QPushButton{    background-color: rgba(0, 192, 213, 0.6);}");
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
