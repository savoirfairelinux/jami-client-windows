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

    ui->chatButton->setCheckable(false);
    ui->hangupButton->setCheckable(false);
    ui->noMicButton->setCheckable(false);
    ui->noVideoButton->setCheckable(false);
    ui->recButton->setCheckable(false);
    ui->holdButton->setCheckable(false);
    ui->transferCallButton->setVisible(false);

    ui->onHoldLabel->setVisible(false);


    ui->transferCallButton->setCheckable(true);

    contactPicker_->setVisible(false);
    contactPicker_->setTitle(QObject::tr("Select peer to transfer to"));

    connect(contactPicker_, &ContactPicker::contactWillDoTransfer, this, &VideoOverlay::slotWillDoTransfer);

    initializeBtnWithIconImageContainers();
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

bool VideoOverlay::on_hangupButton_selected(bool isToBeSelected)
{
    auto convInfo = Utils::getSelectedConversation();
    if (!convInfo.uid.empty()) {
        auto& callId = convInfo.callId;
        auto callModel = LRCInstance::getCurrentCallModel();
        if (callModel->hasCall(callId)) {
            callModel->hangUp(callId);
        }
    }
    return isToBeSelected;
}

bool VideoOverlay::on_chatButton_selected(bool isToBeSelected)
{
    emit setChatVisibility(isToBeSelected);

    return isToBeSelected;
}

bool VideoOverlay::on_holdButton_selected(bool isToBeSelected)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->togglePause(callId_);
    }
    //emit that the hold button status changed
    emit HoldStatusChanged(isToBeSelected);
    ui->onHoldLabel->setVisible(isToBeSelected);

    return isToBeSelected;
}

bool VideoOverlay::on_noMicButton_selected(bool isToBeSelected)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::AUDIO);
    }
    return isToBeSelected;
}

bool VideoOverlay::on_noVideoButton_selected(bool isToBeSelected)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleMedia(callId_, lrc::api::NewCallModel::Media::VIDEO);
    }

    return isToBeSelected;
}

bool VideoOverlay::on_recButton_selected(bool isToBeSelected)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId_)) {
        callModel->toggleAudioRecord(callId_);
    }

    return isToBeSelected;
}

void
VideoOverlay::setTransferCallAvailability(bool visible)
{
    ui->transferCallButton->setVisible(visible);
}

bool VideoOverlay::on_transferCallButton_selected(bool isToBeSelected)
{
    if (callId_.empty() || !isToBeSelected) {
        return false;
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

    return isToBeSelected;
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
    std::map<QPushButton*, QPixmap>::iterator iter = originalIconImageMap_.find(btn);
    if (iter == originalIconImageMap_.end())
        return false;
    switch (event->type()) {
    case QMouseEvent::HoverEnter:
        isBtnHovered_[btn] = true;
        if (!isBtnSelected_[btn]) {
            //set to normal tint
            btn->setIcon(QIcon(tintIconImageMap_[btn]));
        } else {
            //set to checked tint
            btn->setIcon(QIcon(tintCheckedIconImageMap_[btn]));
        }
        break;

    case QMouseEvent::HoverLeave:
        isBtnHovered_[btn] = false;
        if (!isBtnSelected_[btn]) {
            //set to or normal original
            btn->setIcon(QIcon(originalIconImageMap_[btn]));
        } else {
            //set to checked original
            btn->setIcon(QIcon(originalCheckedIconImageMap_[btn]));
        }
        break;
    case QMouseEvent::MouseButtonPress:
        //see if it's selected now
        // call the press function according to toggle of select status
        //set the final select status according to the fact
        isBtnSelected_[btn] = (this->*btnPressCallbacks_[btn])(!isBtnSelected_[btn]);
        //set the button icon according to whether it's supposed to be check and if it's hovered
        if (isBtnHovered_[btn]) {
            if (!isBtnSelected_[btn]) {
                //set to normal tint
                btn->setIcon(QIcon(tintIconImageMap_[btn]));
            } else {
                //set to checked tint
                btn->setIcon(QIcon(tintCheckedIconImageMap_[btn]));
            }
        } else {
            if (!isBtnSelected_[btn]) {
                //set to or normal original
                btn->setIcon(QIcon(originalIconImageMap_[btn]));
            } else {
                //set to checked original
                btn->setIcon(QIcon(originalCheckedIconImageMap_[btn]));
            }
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

void VideoOverlay::initializeBtnWithIconImageContainers()
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

    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->noMicButton, &VideoOverlay::on_noMicButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->noVideoButton, &VideoOverlay::on_noVideoButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->holdButton, &VideoOverlay::on_holdButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->recButton, &VideoOverlay::on_recButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->hangupButton, &VideoOverlay::on_hangupButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->chatButton, &VideoOverlay::on_chatButton_selected));
    btnPressCallbacks_.insert(std::pair<QPushButton*, BtnCallback>(ui->transferCallButton, &VideoOverlay::on_transferCallButton_selected));

    //clear containers
    originalIconImageMap_.clear();
    originalCheckedIconImageMap_.clear();
    tintIconImageMap_.clear();
    tintCheckedIconImageMap_.clear();
    isBtnSelected_.clear();
    isBtnHovered_.clear();

    for (auto go : originalIconImagePathMap) {
        //initialize select and hover state to false in setup map container
        isBtnSelected_[go.first] = false;
        isBtnHovered_[go.first] = false;
        //store original image icon
        originalIconImageMap_[go.first] = QPixmap(go.second);
        //Generate tint images for original state
        tintIconImageMap_[go.first] = Utils::generateTintedPixmap(go.second, QColor(0, 255, 0));
    }

    for (auto go : originalCheckedIconImagePathMap) {
        QPixmap temp = Utils::generateReflectededPixmap(go.second, QColor(0, 255, 0));
        //store checked original image
        originalCheckedIconImageMap_[go.first] = temp;
        //Generate tint images for original checked state
        tintCheckedIconImageMap_[go.first] = Utils::generateTintedPixmap(temp, QColor(0, 255, 0));
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
