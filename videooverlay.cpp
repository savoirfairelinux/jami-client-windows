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
    contactPicker_->setTitle(QObject::tr("Select peer to transfer to"));

    sipInputPanel_->setVisible(false);

    connect(contactPicker_, &ContactPicker::contactWillJoinConference,
            this, &VideoOverlay::slotWillJoinConference);
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
VideoOverlay::callStarted(const std::string& callId)
{
    callId_ = callId;
    setTime();
    connect(oneSecondTimer_, &QTimer::timeout, this, &VideoOverlay::setTime);
    oneSecondTimer_->start(1000);
    showOverlay();
    fadeTimer_.start(startfadeOverlayTime_);
}

void
VideoOverlay::setName(const QString& name)
{
    ui->nameLabel->setText(name);
}

void
VideoOverlay::setPauseState(bool state)
{
    ui->onHoldLabel->setVisible(state);
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
        auto timeString = LRCInstance::getCurrentCallModel()->getFormattedCallDuration(callId_);
        ui->timerLabel->setText(QString::fromStdString(timeString));
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
    if (!LRCInstance::getCurrentCallModel()->hasCall(callId_)) {
        return false;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId_);
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

bool
VideoOverlay::getShowChatView()
{
    return ui->chatButton->isChecked();
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
    auto callModel = LRCInstance::getCurrentCallModel();
    bool onHold { false };
    if (callModel->hasCall(callId_)) {
        callModel->togglePause(callId_);
        onHold = callModel->getCall(callId_).status == lrc::api::call::Status::PAUSED;
    }
    ui->onHoldLabel->setVisible(onHold);
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
    emit videoMuteStateChanged(checked);
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
VideoOverlay::setTransferCallAndSIPPanelAvailability(bool visible)
{
    ui->transferCallButton->setVisible(visible);
    ui->sipInputPanelButton->setVisible(visible);
}

void
VideoOverlay::on_addToConferenceButton_toggled(bool checked)
{
    if (callId_.empty() || !checked) {
        return;
    }
    contactPicker_->setType(ContactPicker::Type::CONFERENCE);

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
VideoOverlay::slotWillDoTransfer(const std::string& callId, const std::string& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->transferCallButton->setChecked(false);
    ui->transferCallButton->resetToOriginal();
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
    auto callModel = LRCInstance::getCurrentCallModel();
    contactPicker_->hide();
    ui->addToConferenceButton->setChecked(false);
    ui->addToConferenceButton->resetToOriginal();

    callModel->callAndAddParticipant(contactUri, callId, false);
}

void
VideoOverlay::setCurrentSelectedCalleeDisplayName(const QString& CalleeDisplayName)
{
    contactPicker_->setCurrentCalleeDisplayName(CalleeDisplayName);
}

void
VideoOverlay::resetOverlay(bool isAudioMuted, bool isVideoMuted, bool isRecording, bool isHolding, bool isAudioOnly)
{
    //Set irrelevant buttons invisible
    ui->noVideoButton->setVisible(!isAudioOnly);
    // Block the signals of buttons
    Utils::whileBlocking(ui->noMicButton)->setOverlayButtonChecked(isAudioMuted);
    Utils::whileBlocking(ui->noVideoButton)->setOverlayButtonChecked(isVideoMuted);
    Utils::whileBlocking(ui->recButton)->setOverlayButtonChecked(isRecording);
    Utils::whileBlocking(ui->holdButton)->setOverlayButtonChecked(isHolding);
    Utils::whileBlocking(ui->onHoldLabel)->setVisible(isHolding);
}

void
VideoOverlay::on_sipInputPanelButton_toggled(bool checked)
{
    if (callId_.empty() || !checked) {
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
    switch (id)
    {
    case 10:
        LRCInstance::getCurrentCallModel()->playDTMF(callId_, "#");
        break;
    case 11:
        LRCInstance::getCurrentCallModel()->playDTMF(callId_, "*");
        break;
    default:
        LRCInstance::getCurrentCallModel()->playDTMF(callId_, std::to_string(id));
        break;
    }
}
