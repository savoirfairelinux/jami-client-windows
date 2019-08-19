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

#include "videoview.h"
#include "lrcinstance.h"
#include "contactpicker.h"
#include "utils.h"

#include <QTime>
#include <QMouseEvent>

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

    ui->transferCallButton->setVisible(false);
    ui->transferCallButton->setCheckable(true);

    contactPicker_->setVisible(false);
    contactPicker_->setTitle(QObject::tr("Select peer to transfer to"));

    connect(ui->transferCallButton, &QPushButton::toggled, this, &VideoOverlay::on_transferButton_toggled);
    connect(contactPicker_, &ContactPicker::contactWillDoTransfer, this, &VideoOverlay::on_transferCall_requested);

    initializeIconImageContainers();
    initializeBtnEventListener();
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

void VideoOverlay::on_holdButton_toggled(bool checked)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->togglePause(callId_);
        bool onHold = callModel->getCall(callId_).status == lrc::api::call::Status::PAUSED;
    }
    //emit that the hold button status changed
    emit HoldStatusChanged(checked);
    ui->onHoldLabel->setVisible(checked);

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
    contactPicker_->setType(ContactPicker::Type::TRANSFER);

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
VideoOverlay::on_transferCall_requested(const std::string& callId, const std::string& contactUri)
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

void VideoOverlay::initializeBtnEventListener()
{
    ui->noMicButton->installEventFilter(this);
    ui->noVideoButton->installEventFilter(this);
    ui->holdButton->installEventFilter(this);
    ui->recButton->installEventFilter(this);
    ui->hangupButton->installEventFilter(this);
    ui->chatButton->installEventFilter(this);
    ui->transferCallButton->installEventFilter(this);
}

bool VideoOverlay::setButtonsIconForEvent(QObject* target, QEvent* event)
{
    QPushButton* btn = (QPushButton*)target;
    if (!btn)
        return false;
    switch (event->type()) {
    case QMouseEvent::HoverEnter:
        if (btn->isChecked()) {
            btn->setIcon(QIcon(tintCheckedIconImageMap_[btn]));
        } else {
            btn->setIcon(QIcon(tintIconImageMap_[btn]));
        }
        break;

    case QMouseEvent::HoverLeave:
        if (btn->isChecked()) {
            btn->setIcon(QIcon(originalCheckedIconImageMap_[btn]));
        } else {
            btn->setIcon(QIcon(originalIconImageMap_[btn]));
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

bool VideoOverlay::eventFilter(QObject* target, QEvent* event)
{
    bool isGot = setButtonsIconForEvent(target, event);
    if (isGot)
    {
        return true;
    }
    else
    {
        return QWidget::eventFilter(target, event);
    }

}

void VideoOverlay::initializeIconImageContainers()
{
    std::map<QPushButton*, QString> originalIconImagePathMap;
    std::map<QPushButton*, QString> originalCheckedIconImagePathMap;

    originalIconImagePathMap[ui->noMicButton] = QString(":/images/icons/ic_mic_white_24dp.png");
    originalIconImagePathMap[ui->noVideoButton] = QString(":/images/icons/ic_videocam_white.png");
    originalIconImagePathMap[ui->holdButton] = QString(":/images/icons/ic_pause_white_24dp.png");
    originalIconImagePathMap[ui->recButton] = QString(":/images/icons/ic_voicemail_white_24dp_2x.png");
    originalIconImagePathMap[ui->hangupButton] = QString(":/images/icons/ic_close_white_24dp.png");
    originalIconImagePathMap[ui->chatButton] = QString(":/images/icons/ic_chat_white_24dp.png");
    originalIconImagePathMap[ui->transferCallButton] = QString(":/images/icons/ic_call_transfer_white_24px.png");

    originalCheckedIconImagePathMap[ui->noMicButton] = QString(":/images/icons/ic_mic_off_white_24dp.png");
    originalCheckedIconImagePathMap[ui->noVideoButton] = QString(":/images/icons/ic_videocam_off_white_24dp.png");
    originalCheckedIconImagePathMap[ui->holdButton] = QString(":/images/icons/ic_pause_white_24dp.png");
    originalCheckedIconImagePathMap[ui->recButton] = QString(":/images/icons/ic_voicemail_white_24dp_2x.png");
    originalCheckedIconImagePathMap[ui->hangupButton] = QString(":/images/icons/ic_close_white_24dp.png");
    originalCheckedIconImagePathMap[ui->chatButton] = QString(":/images/icons/ic_chat_white_24dp.png");
    originalCheckedIconImagePathMap[ui->transferCallButton] = QString(":/images/icons/ic_call_transfer_white_24px.png");

    //clear containers
    originalIconImageMap_.clear();
    originalCheckedIconImageMap_.clear();
    tintIconImageMap_.clear();
    tintCheckedIconImageMap_.clear();

    //store original image icon
    for (auto go : originalIconImagePathMap) {
        originalIconImageMap_[go.first] = QPixmap(go.second);
    }

    //store checked original image
    for (auto go : originalCheckedIconImagePathMap) {
        originalCheckedIconImageMap_[go.first] = QPixmap(go.second);
    }

    //Generate tint images for original state
    for (auto go : originalIconImagePathMap) {
        tintIconImageMap_[go.first] = Utils::generateTintedPixmap(go.second, QColor(0, 255, 0));
    }

    //Generate tint images for original checked state
    for (auto go : originalCheckedIconImagePathMap) {
        tintCheckedIconImageMap_[go.first] = Utils::generateTintedPixmap(go.second, QColor(0, 255, 0));
    }
}

void
VideoOverlay::setCurrentSelectedCalleeDisplayName(const QString& CalleeDisplayName)
{
    contactPicker_->setCurrentCalleeDisplayName(CalleeDisplayName);
}

void VideoOverlay::resetOverlay(bool isAudioMuted, bool isVideoMuted, bool isRecording, bool isHolding, bool isAudioOnly)
{
    //Set irrelevant buttons invisible
    ui->noVideoButton->setVisible(!isAudioOnly);
    // Block the signals of buttons
    Utils::whileBlocking(ui->noMicButton)->setChecked(isAudioMuted);
    Utils::whileBlocking(ui->noVideoButton)->setChecked(isVideoMuted);
    Utils::whileBlocking(ui->recButton)->setChecked(isRecording);
    Utils::whileBlocking(ui->holdButton)->setChecked(isHolding);
    Utils::whileBlocking(ui->onHoldLabel)->setVisible(isHolding);
}
