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
#include "popupdialog.h"
#include "sipinputpanel.h"
#include "utils.h"

#include <QTime>

VideoOverlay::VideoOverlay(QWidget* parent)
    : FadeOutable(parent)
    , ui(new Ui::VideoOverlay)
    , oneSecondTimer_(new QTimer(this))
{
    ui->setupUi(this);

    ui->bottomButtons->setMouseTracking(true);

    ui->chatButton->setCheckable(true);

    ui->onHoldLabel->setVisible(false);

    ui->transferCallButton->setVisible(false);
    ui->transferCallButton->setCheckable(true);

    ui->addToConferenceButton->setVisible(true);
    ui->addToConferenceButton->setCheckable(true);

    ui->holdButton->setVisible(false);

}

VideoOverlay::~VideoOverlay()
{
    delete ui;
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
    oneSecondTimer_->start(20);
    resetToFadeIn(true);

    // close chat panel
    emit setChatVisibility(false);

    auto& accInfo = LRCInstance::accountModel().getAccountInfo(accountId_);

    auto& convModel = accInfo.conversationModel;

    auto call = LRCInstance::getCallInfoForConversation(convInfo);
    if (!call) {
        return;
    }

    auto bestName = Utils::bestNameForConversation(convInfo, *convModel);
    currentBestCalleeDisplayName_ = bestName;
    ui->nameLabel->setText(bestName);

    bool isPaused = call->status == lrc::api::call::Status::PAUSED;
    bool isAudioOnly = call->isAudioOnly && !isPaused;
    bool isAudioMuted = call->audioMuted && (call->status != lrc::api::call::Status::PAUSED);
    bool isVideoMuted = call->videoMuted && !isPaused && !call->isAudioOnly;
    bool isRecording = accInfo.callModel->isRecording(convInfo.callId);

    //Set irrelevant buttons invisible
    ui->noVideoButton->setVisible(!isAudioOnly);

    // Block the signals of buttons
    ui->noMicButton->setOverlayButtonChecked(isAudioMuted);
    ui->noVideoButton->setOverlayButtonChecked(isVideoMuted);
    ui->recButton->setOverlayButtonChecked(isRecording);
    ui->holdButton->setOverlayButtonChecked(isPaused);
    ui->onHoldLabel->setVisible(isPaused);

    bool isSIP = accInfo.profileInfo.type == lrc::api::profile::Type::SIP;
    ui->addToConferenceButton->setVisible(!isSIP);
    ui->transferCallButton->setVisible(isSIP);
    ui->sipInputPanelButton->setVisible(isSIP);

    // only show the hold/pause button for non-conference calls
    ui->holdButton->setVisible(convInfo.confId.isEmpty());
}

void
VideoOverlay::setTime()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    if (callInfo.status == lrc::api::call::Status::IN_PROGRESS ||
        callInfo.status == lrc::api::call::Status::PAUSED) {
        auto timeString = LRCInstance::getCurrentCallModel()->getFormattedCallDuration(callId);
        ui->timerLabel->setText(timeString);
    }
}

bool
VideoOverlay::shouldFadeOut()
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
        return false;
    }
    auto callInfo = LRCInstance::getCurrentCallModel()->getCall(callId);
    bool hoveringOnButtons = ui->bottomButtons->underMouse() || ui->topInfoBar->underMouse();
    return not (hoveringOnButtons ||
               (callInfo.status == lrc::api::call::Status::PAUSED) ||
                popUpShown_);
}

void
VideoOverlay::updateGeometry(const QSize& size)
{
    resize(size);
    QRect endRect = rect();
    setFadeOutStartLocation(endRect);
    endRect.setHeight(endRect.height() + ui->bottomButtons->height() + ui->bottomButtons->height());
    endRect.setY(endRect.y() - ui->bottomButtons->height() - ui->bottomButtons->height());
    setFadeOutEndLocation(endRect);
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
    if (!convInfo.uid.isEmpty()) {
        auto callModel = LRCInstance::getCurrentCallModel();
        if (callModel->hasCall(convInfo.callId)) {
            // Store the last remaining participant of the conference
            // so we can switch the smartlist index after termination.
            if (!convInfo.confId.isEmpty()) {
                auto callList = LRCInstance::getAPI().getConferenceSubcalls(convInfo.confId);
                if (callList.size() == 2) {
                    for (const auto& cId : callList) {
                        if (cId != convInfo.callId) {
                            LRCInstance::instance().pushLastConferencee(convInfo.confId, cId);
                        }
                    }
                }
            }
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
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
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
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
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
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
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
    if (callId.isEmpty() || !LRCInstance::getCurrentCallModel()->hasCall(callId)) {
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
    if ( callId.isEmpty() ||
         !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
         !checked) {
        return;
    }

    contactPickerPopup(SmartListModel::Type::CONFERENCE, ui->addToConferenceButton);
}

void
VideoOverlay::on_transferCallButton_toggled(bool checked)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() ||
        !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
        !checked) {
        return;
    }

    contactPickerPopup(SmartListModel::Type::TRANSFER, ui->transferCallButton);
}

void
VideoOverlay::slotWillDoTransfer(const QString& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();

    ui->transferCallButton->setChecked(false);
    ui->transferCallButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.isEmpty()) {
        return;
    }
    auto callId = conversation.confId.isEmpty() ? conversation.callId : conversation.confId;

    QString destCallId;

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
VideoOverlay::slotContactWillJoinConference(const QString& contactUri)
{
    auto callModel = LRCInstance::getCurrentCallModel();

    ui->addToConferenceButton->setChecked(false);
    ui->addToConferenceButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    auto call = LRCInstance::getCallInfoForConversation(conversation);
    if (!call) {
        return;
    }
    callModel->callAndAddParticipant(contactUri, call->id, call->isAudioOnly);
}

void
VideoOverlay::slotCallWillJoinConference(const QString& callId)
{
    auto callModel = LRCInstance::getCurrentCallModel();

    ui->addToConferenceButton->setChecked(false);
    ui->addToConferenceButton->resetToOriginal();

    auto conversation = LRCInstance::getCurrentConversation();
    if (conversation.uid.isEmpty()) {
        return;
    }
    auto thisCallId = conversation.confId.isEmpty() ? conversation.callId : conversation.confId;

    callModel->joinCalls(thisCallId, callId);
}

void
VideoOverlay::on_sipInputPanelButton_toggled(bool checked)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if (callId.isEmpty() ||
        !LRCInstance::getCurrentCallModel()->hasCall(callId) ||
        !checked) {
        return;
    }

    SIPInputPanelWidget sipInputPanel(this);

    connect(&sipInputPanel, &SIPInputPanelWidget::sipInputPanelClicked,
        this, &VideoOverlay::slotSIPInputPanelClicked);

    QPoint globalPos_button = mapToGlobal(ui->sipInputPanelButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    if (auto container = sipInputPanel.getContainer().toStrongRef()) {

        container->move(globalPos_button.x(),
                        globalPos_bottomButtons.y() - sipInputPanel.height() - popupMargin_);

        // receive the signal that ensure the button checked status is correct and contactpicker
        // is properly hidden
        Utils::oneShotConnect(container.data(), &PopupDialog::willClose,
            [this](QMouseEvent* event) {
                // check if current mouse position is on button
                auto relativeClickPos = ui->sipInputPanelButton->mapFromGlobal(event->globalPos());
                if (!ui->sipInputPanelButton->rect().contains(relativeClickPos)) {
                    ui->sipInputPanelButton->setChecked(false);
                    ui->sipInputPanelButton->resetToOriginal();
                }
            });

        // for esc key, receive reject signal
        Utils::oneShotConnect(container.data(), &QDialog::rejected,
            [this] {
                ui->sipInputPanelButton->setChecked(false);
                ui->sipInputPanelButton->resetToOriginal();
            });

        popUpShown_ = true;
        container->exec();
        popUpShown_ = false;
    }
}

void
VideoOverlay::slotSIPInputPanelClicked(const int& id)
{
    auto callId = LRCInstance::getCallIdForConversationUid(convUid_, accountId_);
    if ( callId.isEmpty() ||
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
        // id from 12 to 15 are A,B,C,D
        if (id >= 12) {
            //ASCII Key_A = 65
            LRCInstance::getCurrentCallModel()->playDTMF(callId, QString(char(id - 12 + 65)));
        } else {
            LRCInstance::getCurrentCallModel()->playDTMF(callId, QString::number(id));
        }
        break;
    }
}

void
VideoOverlay::contactPickerPopup(SmartListModel::Type type, OverlayButton* locateButton)
{
    ContactPicker contactPicker(this);
    contactPicker.setCurrentCalleeDisplayName(currentBestCalleeDisplayName_);
    contactPicker.setType(type);
    contactPicker.setTitle(type == SmartListModel::Type::CONFERENCE ? QObject::tr("Add to conference") : QObject::tr("Transfer this call"));

    connect(&contactPicker, &ContactPicker::contactWillJoinConference,
        this, &VideoOverlay::slotContactWillJoinConference);
    connect(&contactPicker, &ContactPicker::callWillJoinConference,
        this, &VideoOverlay::slotCallWillJoinConference);
    connect(&contactPicker, &ContactPicker::contactWillDoTransfer,
        this, &VideoOverlay::slotWillDoTransfer);

    QPoint globalPos_button = mapToGlobal(locateButton->pos());
    QPoint globalPos_bottomButtons = mapToGlobal(ui->bottomButtons->pos());

    if (auto container = contactPicker.getContainer().toStrongRef()) {

        container->move(globalPos_button.x(),
            globalPos_bottomButtons.y() - contactPicker.height() - popupMargin_);

        // receive the signal that ensure the button checked status is correct and contactpicker
        // is properly hidden
        Utils::oneShotConnect(container.data(), &PopupDialog::willClose,
            [locateButton, this](QMouseEvent* event) {
                // check if current mouse position is on button
                auto relativeClickPos = locateButton->mapFromGlobal(event->globalPos());
                if (!locateButton->rect().contains(relativeClickPos)) {
                    locateButton->setChecked(false);
                    locateButton->resetToOriginal();
                }
            });

        // for esc key, receive reject signal
        Utils::oneShotConnect(container.data(), &QDialog::rejected,
            [this] {
                ui->addToConferenceButton->setChecked(false);
                ui->addToConferenceButton->resetToOriginal();
            });

        popUpShown_ = true;
        container->exec();
        popUpShown_ = false;
    }
}
