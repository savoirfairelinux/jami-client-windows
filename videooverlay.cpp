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

// Client
#include "contactpicker.h"

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
    transferDialog_(new CallUtilsDialog()),
    qualityDialog_(new QualityDialog())
{
    ui->setupUi(this);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->noMicButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    transferDialog_->setAttribute(Qt::WA_TranslucentBackground);
    connect(transferDialog_, &CallUtilsDialog::isVisible, [this] (bool visible) {
        dialogVisible_ = visible;
    });

    qualityDialog_->setAttribute(Qt::WA_TranslucentBackground);
    connect(qualityDialog_, &QualityDialog::isVisible, [this] (bool visible) {
        dialogVisible_ = visible;
    });
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
    delete transferDialog_;
    delete qualityDialog_;
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setTime(const QString& time)
{
    ui->timerLabel->setText(time);
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
VideoOverlay::on_hangupButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvId();
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
VideoOverlay::on_transferButton_clicked()
{
    transferDialog_->setConfMode(false);
    auto pos = this->mapToGlobal(ui->transferButton->pos());
    transferDialog_->move(pos.x()
                          - transferDialog_->size().width()/2
                          + ui->transferButton->size().width()/2,
                          pos.y() - (transferDialog_->height()));
    transferDialog_->show();
}

void
VideoOverlay::on_addPersonButton_clicked()
{
    transferDialog_->setConfMode(true);
    auto pos = this->mapToGlobal(ui->addPersonButton->pos());
    transferDialog_->move(pos.x()
                          - transferDialog_->size().width()/2
                          + ui->addPersonButton->size().width()/2,
                          pos.y() - (transferDialog_->height()));
    transferDialog_->show();
}

void
VideoOverlay::on_holdButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvId();
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
    auto selectedConvUid = LRCInstance::getSelectedConvId();
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
    auto selectedConvUid = LRCInstance::getSelectedConvId();
    auto conversation = Utils::getConversationFromUid(selectedConvUid,
        *LRCInstance::getCurrentConversationModel());
    auto& callId = conversation->callId;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        ui->noVideoButton->setChecked(callModel->getCall(callId).videoMuted);
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::VIDEO);
    }
}


void VideoOverlay::on_joinButton_clicked()
{
    // TODO:(newlrc) conferences
    //CallModel::instance().selectedCall()->joinToParent();
}

void
VideoOverlay::on_qualityButton_clicked()
{
    auto pos = this->mapToGlobal(ui->qualityButton->pos());
    qualityDialog_->move(pos.x()
              - qualityDialog_->size().width()/2
              + ui->qualityButton->size().width()/2,
              pos.y() - (qualityDialog_->height()));
    qualityDialog_->show();
}

void
VideoOverlay::on_recButton_clicked()
{
    auto selectedConvUid = LRCInstance::getSelectedConvId();
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
