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
#include <QMessagebox>

#include "lrcinstance.h"
#include "utils.h"

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    oneSecondTimer_(new QTimer(this)),
    listOfContactForTrans_ (new ContactPicker(this))
{
    ui->setupUi(this);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->recButton->setVisible(false);

    ui->transferCallButton->setVisible(false);
    ui->transferCallButton->setCheckable(true);

    listOfContactForTrans_->setVisible(false);
    listOfContactForTrans_->setTitle("Select Peer to Tranfer");
    //listOfContactForTrans_->setModal(true);
    //listOfContactForTrans_->setWindowModality(Qt::ApplicationModal);

    connect(ui->transferCallButton, &QPushButton::toggled, this, &VideoOverlay::on_transferButton_toggled);
    connect(listOfContactForTrans_, &ContactPicker::contactWillDoBlindTransfer, this, &VideoOverlay::on_transferCall_requested);
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
VideoOverlay::on_noMicButton_toggled(bool checked)
{
    bool btn_status = false;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::AUDIO);
        btn_status = callModel->getCall(callId_).audioMuted;
    }
}

void
VideoOverlay::on_noVideoButton_toggled(bool checked)
{
    bool btn_status = false;
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::VIDEO);
        btn_status = callModel->getCall(callId_).videoMuted;
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

void
VideoOverlay::setTransferCallAvailability(bool visible)
{
    ui->transferCallButton->setVisible(visible);
}

void
VideoOverlay::on_transferButton_toggled(bool checked)
{
    if (callId_.empty() || !checked) {
        return;
    }
    listOfContactForTrans_->setRegexMatchExcept();

    QPoint globalPos_button = mapToGlobal(ui->transferCallButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    listOfContactForTrans_->move(globalPos_button.x(), globalPos_bottomButtons.y() - listOfContactForTrans_->height());
    listOfContactForTrans_->exec();
}

void
VideoOverlay::on_transferCall_requested(const std::string& callId, const std::string& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    auto thisCallIsAudioOnly = callModel->getCall(callId).isAudioOnly;
    //auto thisCallIsAudioOnly = callModel->getCall(callId).isAudioOnly;
    //QMessageBox::information(0,QString::fromStdString(contactUri), QString::fromStdString(contactUri));
    //callModel->transfer(callId, contactUri);
    listOfContactForTrans_->done(0);
    std::string destCallId;
    //ui->transferCallButton->setChecked(false);

    try {
        auto callInfo = callModel->getCallFromURI(contactUri);
        destCallId = callInfo.id;
    } catch(std::exception& e) {
        destCallId = "";
    }
    if (destCallId.size() == 0) {
        callModel->transfer(callId, "sip:" + contactUri);
        callModel->hangUp(callId);
    }else{
        callModel->transferToCall(callId, destCallId);
        callModel->hangUp(callId);
    }

    /*auto attendedCallId = callModel->createCall(contactUri, thisCallIsAudioOnly);
    QObject::connect(callModel, &lrc::api::NewCallModel::callStatusChanged,
                    [this, attendedCallId, callId](const std::string& callId_coming) {
                        if (callId_coming != attendedCallId)
                            return;
                        using namespace lrc::api::call;
                        auto call = LRCInstance::getCurrentCallModel()->getCall(callId_coming);
                        switch (call.status) {
                        case Status::IN_PROGRESS:
                        {
                            qDebug() << "attending to transfer the call" << QString::fromStdString(callId_coming);
                            LRCInstance::getCurrentCallModel()->transferToCall(callId, attendedCallId);
                            LRCInstance::getCurrentCallModel()->hangUp(callId);
                            return;
                        }
                        default:
                            qDebug() << "failed to add to transfer the call" << QString::fromStdString(callId_coming);
                            break;
                        }
                    });*/
    //if(callModel->hasCall(callId))

    //callModel->transfer(callId, "sip:" + contactUri);
    //callModel->hangUp(callId);
}

inline const QRect
VideoOverlay::getListOfContactRect()
{
    return listOfContactForTrans_->rect();
}

void
VideoOverlay::resetTransButtonClicked()
{
    listOfContactForTrans_->done(0);
    ui->transferCallButton->setChecked(false);
}

void
VideoOverlay::setCurrentSelectedCalleeDisplayName(const QString& CalleeDisplayName)
{
    listOfContactForTrans_->setCurrentCalleeDisplayName(CalleeDisplayName);
}
