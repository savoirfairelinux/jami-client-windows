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
#include "sipinputpanel.h"
#include "utils.h"

#include <QTime>
#include <QMouseEvent>
#include <QGraphicsOpacityEffect>

VideoOverlay::VideoOverlay(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::VideoOverlay),
    oneSecondTimer_(new QTimer(this)),
    contactPicker_(new ContactPicker(this)),
    sipInputPanel_(new SipInputPanel(this))
{
    ui->setupUi(this);

    setAttribute(Qt::WA_NoSystemBackground);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    ui->transferCallButton->setVisible(false);
    ui->transferCallButton->setCheckable(true);

    ui->addToConferenceButton->setVisible(true);
    ui->addToConferenceButton->setCheckable(true);

    contactPicker_->setVisible(false);

    sipInputPanel_->setVisible(false);

    connect(contactPicker_, &ContactPicker::contactWillJoinConference,
            this, &VideoOverlay::slotContactWillJoinConference);
    connect(contactPicker_, &ContactPicker::callWillJoinConference,
            this, &VideoOverlay::slotCallWillJoinConference);
    connect(contactPicker_, &ContactPicker::contactWillDoTransfer,
            this, &VideoOverlay::slotWillDoTransfer);
    connect(sipInputPanel_, &SipInputPanel::sipInputPanelClicked,
            this, &VideoOverlay::slotSIPInputPanelClicked);

    // fading mechanism
    auto effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(maxOverlayOpacity_);
    this->setGraphicsEffect(effect);
    fadeAnim_ = new QPropertyAnimation(this);
    fadeAnim_->setTargetObject(effect);
    fadeAnim_->setPropertyName("opacity");
    fadeAnim_->setDuration(fadeOverlayTime_);
    fadeAnim_->setStartValue(effect->opacity());
    fadeAnim_->setEndValue(0);
    fadeAnim_->setEasingCurve(QEasingCurve::OutQuad);
    // Setup the timer to start the fade when the mouse stops moving
    fadeTimer_.setSingleShot(true);
    connect(&fadeTimer_, SIGNAL(timeout()), this, SLOT(fadeOverlayOut()));
}

VideoOverlay::~VideoOverlay()
{
    delete ui;
    delete fadeAnim_;
}

void
VideoOverlay::enterEvent(QEvent* event)
{
    Q_UNUSED(event);
    showOverlay();
}

void
VideoOverlay::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    fadeTimer_.start(startfadeOverlayTime_);
}

void
VideoOverlay::mouseMoveEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // start/restart the timer after which the overlay will fade
    if (fadeTimer_.isActive()) {
        showOverlay();
    } else {
        fadeTimer_.start(startfadeOverlayTime_);
    }
    event->ignore();
}

void
VideoOverlay::showOverlay()
{
    fadeAnim_->stop();
    fadeAnim_->targetObject()->setProperty(fadeAnim_->propertyName(),
                                           fadeAnim_->startValue());
}

void
VideoOverlay::fadeOverlayOut()
{
    if (!shouldShowOverlay()) {
        fadeAnim_->start(QAbstractAnimation::KeepWhenStopped);
    }
}

void
VideoOverlay::updateCall(const conversation::Info& convInfo)
{
    accountId_ = convInfo.accountId;
    convUid_ = convInfo.uid;

    setTime();
    QObject::disconnect(oneSecondTimer_);
    QObject::connect(oneSecondTimer_, &QTimer::timeout,
                     this, &VideoOverlay::setTime);
    oneSecondTimer_->start(1000);
    showOverlay();
    fadeTimer_.start(startfadeOverlayTime_);

    // close chat panel
    emit setChatVisibility(false);

    auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId_);

    auto& convModel = accInfo.conversationModel;

    auto call = LRCInstance::getCallInfoForConversation(convInfo);
    if (!call) {
        return;
    }

    auto bestName = QString::fromStdString(
        Utils::bestNameForConversation(convInfo, *convModel));
    contactPicker_->setCurrentCalleeDisplayName(bestName);
    ui->nameLabel->setText(bestName);

    bool isPaused = call->status == lrc::api::call::Status::PAUSED;
    bool isAudioOnly = call->isAudioOnly && !isPaused;
    bool isAudioMuted = call->audioMuted && (call->status != lrc::api::call::Status::PAUSED);
    bool isVideoMuted = call->videoMuted && !isPaused && !call->isAudioOnly;
    bool isRecording = accInfo.callModel->isRecording(convInfo.callId);

    //Set irrelevant buttons invisible
    ui->noVideoButton->setVisible(!isAudioOnly);

    // Block the signals of buttons
    Utils::whileBlocking(ui->noMicButton)->setOverlayButtonChecked(isAudioMuted);
    Utils::whileBlocking(ui->noVideoButton)->setOverlayButtonChecked(isVideoMuted);
    Utils::whileBlocking(ui->recButton)->setOverlayButtonChecked(isRecording);
    Utils::whileBlocking(ui->holdButton)->setOverlayButtonChecked(isPaused);
    Utils::whileBlocking(ui->onHoldLabel)->setVisible(isPaused);

    bool isSIP = accInfo.profileInfo.type == lrc::api::profile::Type::SIP;
    ui->addToConferenceButton->setVisible(!isSIP);
    ui->transferCallButton->setVisible(isSIP);
    ui->sipInputPanelButton->setVisible(isSIP);
}

void
VideoOverlay::setTime()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    if (callInfo.status == lrc::api::call::Status::IN_PROGRESS ||
        callInfo.status == lrc::api::call::Status::PAUSED) {
        auto timeString = LRCInstance::getCurrentCallModel()->getFormattedCallDuration(callId);
        ui->timerLabel->setText(QString::fromStdString(timeString));
    }
}

bool
VideoOverlay::shouldShowOverlay()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return false;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    bool hoveringOnButtons = ui->bottomButtons->underMouse() || ui->topInfoBar->underMouse();
    return  hoveringOnButtons ||
            (callInfo.status == lrc::api::call::Status::PAUSED) ||
            contactPicker_->isActiveWindow() ||
            sipInputPanel_->isActiveWindow();
}

void
VideoOverlay::simulateShowChatview(bool checked)
{
    ui->chatButton->setChecked(checked);
}

void
VideoOverlay::on_hangupButton_clicked()
{
    auto convInfo = LRCInstance::getCurrentConversation();
    if (!convInfo.uid.empty()) {
        auto callModel = LRCInstance::getCurrentCallModel();
        if (callModel->hasCall(convInfo.callId)) {
            callModel->hangUp(convInfo.callId);
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
VideoOverlay::on_holdButton_toggled(bool checked)
{
    // why is 'checked' unused?
    Q_UNUSED(checked);
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    bool onHold { false };
    if (callModel->hasCall(callId)) {
        callModel->togglePause(callId);
        onHold = callModel->getCall(callId).status == lrc::api::call::Status::PAUSED;
    }
    ui->onHoldLabel->setVisible(onHold);
}

void
VideoOverlay::on_noMicButton_toggled(bool checked)
{
    Q_UNUSED(checked);
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::AUDIO);
    }
}

void
VideoOverlay::on_noVideoButton_toggled(bool checked)
{
    Q_UNUSED(checked);
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleMedia(callId, lrc::api::NewCallModel::Media::VIDEO);
    }
    emit videoMuteStateChanged(checked);
}

void
VideoOverlay::on_recButton_clicked()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callModel = LRCInstance::getCurrentCallModel();
    if (callModel->hasCall(callId)) {
        callModel->toggleAudioRecord(callId);
    }
}

void
VideoOverlay::on_addToConferenceButton_toggled(bool checked)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if ( callId.empty() ||
         !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
         !checked) {
        return;
    }

    contactPicker_->setType(SmartListModel::Type::CONFERENCE);
    contactPicker_->setTitle(QObject::tr("Add to conference"));

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
                ui->addToConferenceButton->resetToOriginal();
            }
        });

    // for esc key, receive reject signal
    Utils::oneShotConnect(contactPicker_, &QDialog::rejected,
        [this] {
            ui->addToConferenceButton->setChecked(false);
            ui->addToConferenceButton->resetToOriginal();
        });

    contactPicker_->show();
}

void
VideoOverlay::on_transferCallButton_toggled(bool checked)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() ||
        !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
        !checked) {
        return;
    }
    contactPicker_->setType(SmartListModel::Type::TRANSFER);
    contactPicker_->setTitle(QObject::tr("Select peer to transfer to"));

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
                ui->transferCallButton->resetToOriginal();
            }
        });

    // for esc key, receive reject signal
    Utils::oneShotConnect(contactPicker_, &QDialog::rejected,
    [this] {
        ui->transferCallButton->setChecked(false);
        ui->transferCallButton->resetToOriginal();
    });

    contactPicker_->show();
}

void
VideoOverlay::slotWillDoTransfer(const std::string& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->transferCallButton->setChecked(false);
    ui->transferCallButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.empty()) {
        return;
    }
    auto callId = conversation.callId;

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
VideoOverlay::slotContactWillJoinConference(const std::string& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->addToConferenceButton->setChecked(false);
    ui->addToConferenceButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.empty()) {
        return;
    }
    auto callId = conversation.confId.empty() ? conversation.callId : conversation.confId;

    callModel->callAndAddParticipant(contactUri, callId, false);
}

void
VideoOverlay::slotCallWillJoinConference(const std::string& callId)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->addToConferenceButton->setChecked(false);
    ui->addToConferenceButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.empty()) {
        return;
    }
    auto thisCallId = conversation.confId.empty() ? conversation.callId : conversation.confId;

    callModel->joinCalls(thisCallId, callId);
}

void
VideoOverlay::on_sipInputPanelButton_toggled(bool checked)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.empty() ||
        !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
        !checked) {
        return;
    }

    QPoint globalPos_button = mapToGlobal(ui->sipInputPanelButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    sipInputPanel_->move(globalPos_button.x(), globalPos_bottomButtons.y() - sipInputPanel_->height());

    // receive the signal that ensure the button checked status is correct and contactpicker
    // is properly hidden
    Utils::oneShotConnect(sipInputPanel_, &SipInputPanel::willClose,
        [this](QMouseEvent *event) {
            sipInputPanel_->hide();
            // check if current mouse position is on button
            auto relativeClickPos = ui->sipInputPanelButton->mapFromGlobal(event->globalPos());
            if (!ui->sipInputPanelButton->rect().contains(relativeClickPos)) {
                ui->sipInputPanelButton->setChecked(false);
                ui->sipInputPanelButton->resetToOriginal();
            }
        });

    // for esc key, receive reject signal
    Utils::oneShotConnect(sipInputPanel_, &QDialog::rejected,
    [this] {
        ui->sipInputPanelButton->setChecked(false);
        ui->sipInputPanelButton->resetToOriginal();
    });

    sipInputPanel_->show();
}

void
VideoOverlay::slotSIPInputPanelClicked(const int& id)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if ( callId.empty() ||
         !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }

    switch (id)
    {
    case 10:
        LRCInstance::getCurrentCallModel()->playDTMF(callId, "#");
        break;
    case 11:
        LRCInstance::getCurrentCallModel()->playDTMF(callId, "*");
        break;
    default:
        LRCInstance::getCurrentCallModel()->playDTMF(callId, std::to_string(id));
        break;
    }
}
