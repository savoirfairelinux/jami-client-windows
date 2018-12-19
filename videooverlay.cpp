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

// LRC
#include "callmodel.h"
#include "contactmethod.h"
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

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->noMicButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    connect(LRCInstance::getCurrentCallModel(), &lrc::api::NewCallModel::callStarted,
        [this](const std::string& tempCallId) { callId = tempCallId; });

    connect(oneSecondTimer_, &QTimer::timeout, this, &VideoOverlay::setTime);
    oneSecondTimer_->start(1000);
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setTime()
{
    if (callId.empty()) { return; }

    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    if(callInfo.status == lrc::api::call::Status::IN_PROGRESS) {

        int numSeconds = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - callInfo.startTime).count();

        QString labelSec;
        QString labelMin;

        int numMinutes = numSeconds / 60;
        int remainder = numSeconds - numMinutes * 60;
        if (remainder < 10) {
            labelSec = ":0" + QString::number(remainder);
        } else {
            labelSec = ":" + QString::number(remainder);
        }

        if (numMinutes < 10) {
            labelMin = "0" + QString::number(numMinutes);
        } else {
            labelMin = QString::number(numMinutes);
        }

        ui->timerLabel->setText(labelMin + labelSec);
    }
}

void VideoOverlay::toggleContextButtons(bool visible)
{
    if (! visible) {
        ui->videoCfgBtn->hide();
    } else {
        ui->videoCfgBtn->show();
    }
}

void
VideoOverlay::setVideoMuteVisibility(bool visible)
{
    ui->noVideoButton->setVisible(visible);
}

bool
VideoOverlay::shouldShowOverlay()
{
    return ui->bottomButtons->underMouse() || ui->topInfoBar->underMouse();
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
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->hangUp(callId);
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
    auto selectedConvUid = LRCInstance::getSelectedConvUid();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        ui->noMicButton->setChecked(callModel->getCall(callId).audioMuted);
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::AUDIO);
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
        ui->noVideoButton->setChecked(callModel->getCall(callId).videoMuted);
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::VIDEO);
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

void VideoOverlay::on_videoCfgBtn_clicked()
{
    emit videoCfgBtnClicked();
}
