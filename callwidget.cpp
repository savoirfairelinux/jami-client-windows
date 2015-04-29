/***************************************************************************
 * Copyright (C) 2011-2015 by Savoir-Faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "callwidget.h"
#include "ui_callwidget.h"


#include "imconversationmanager.h"
#include "instantmessagingmodel.h"
#include "audio/settings.h"
#include "personmodel.h"
#include "fallbackpersoncollection.h"
#include "accountmodel.h"

#include "utils.h"

CallWidget::CallWidget(QWidget *parent) :
    NavWidget(Main ,parent),
    ui(new Ui::CallWidget)
{
    ui->setupUi(this);

    ui->holdButton->setCheckable(true);
    ui->muteMicButton->setCheckable(true);
    ui->muteSpeakerButton->setCheckable(true);
    ui->callInvite->setVisible(false);

    actualCall_ = nullptr;
    videoRenderer_ = nullptr;

    try {
        callModel_ = CallModel::instance();

        connect(callModel_, SIGNAL(incomingCall(Call*)),
                this, SLOT(callIncoming(Call*)));
        connect(callModel_, SIGNAL(callAdded(Call*,Call*)),
                this, SLOT(addedCall(Call*, Call*)));
        connect(callModel_, SIGNAL(callStateChanged(Call*, Call::State)),
                this, SLOT(callStateChanged(Call*, Call::State)));

        ui->callList->setModel(callModel_);

        CategorizedHistoryModel::instance()->
                addCollection<MinimalHistoryBackend>(LoadOptions::FORCE_ENABLED);

        ui->historyList->setModel(CategorizedHistoryModel::instance());
        ui->speakerSlider->setValue(Audio::Settings::instance()->playbackVolume());
        ui->micSlider->setValue(Audio::Settings::instance()->captureVolume());

        findRingAccount();

    } catch (...) {
        qDebug() << "INIT ERROR";
    }
}

CallWidget::~CallWidget()
{
    delete ui;
}

void
CallWidget::findRingAccount() {
    auto a_count = AccountModel::instance()->rowCount();
    auto found = false;
    for (int i = 0; i < a_count; ++i) {
        auto idx = AccountModel::instance()->index(i, 0);
        auto protocol = idx.data(static_cast<int>(Account::Role::Proto));
        if ((Account::Protocol)protocol.toUInt() == Account::Protocol::RING) {
            auto username = idx.data(static_cast<int>(Account::Role::Username));
            ui->ringIdLabel->setText(
                        ui->ringIdLabel->text() + " " + username.toString());
            found = true;
            return;
        }
    }
    if (!found) {
        AccountModel::instance()->add("RING", Account::Protocol::RING);
        AccountModel::instance()->save();
        Utils::CreateStartupLink();
        findRingAccount();
    }
}

void
CallWidget::callIncoming(Call *call)
{
    if (!call->account()->isAutoAnswer())
        ui->callInvite->setVisible(true);
    actualCall_ = call;
}

void
CallWidget::on_acceptButton_clicked()
{
    if (actualCall_ != nullptr)
       actualCall_->performAction(Call::Action::ACCEPT);
    ui->callInvite->setVisible(false);
}

void
CallWidget::on_refuseButton_clicked()
{
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
    actualCall_ = nullptr;
    ui->callInvite->setVisible(false);
}

void
CallWidget::on_holdButton_toggled(bool checked)
{
    Q_UNUSED(checked)
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::HOLD);
}

void
CallWidget::on_hangupButton_clicked()
{
    if (actualCall_ == nullptr)
        return;
    actualCall_->performAction(Call::Action::REFUSE);
}

void
CallWidget::addedCall(Call* call, Call* parent) {
    Q_UNUSED(parent);
    if (call->direction() == Call::Direction::OUTGOING) {
        actualCall_ = call;
    }
}

void
CallWidget::callStateChanged(Call* call, Call::State previousState) {
    Q_UNUSED(previousState)
    ui->callList->setCurrentIndex(callModel_->getIndex(actualCall_));
    if (call->state() == Call::State::OVER) {
        actualCall_ = nullptr;
        ui->videoWidget->hide();
    } else {
        ui->videoWidget->show();
        connect(actualCall_, SIGNAL(isOver(Call*)),
                this, SLOT(callStateChanged(Call*,Call::State::OVER)));
        ui->messageOutput->setModel(
                    IMConversationManager::instance()->getModel(actualCall_));
    }
}

void
CallWidget::on_callList_activated(const QModelIndex &index)
{
    Call *callSelected = callModel_->getCall(index);
    if (actualCall_ != nullptr) {
        if (callSelected == actualCall_)
            return;
        ui->videoWidget->hide();
        actualCall_->performAction(Call::Action::HOLD);
    }
    actualCall_ = callSelected;
    actualCall_->performAction(Call::Action::HOLD);
    ui->videoWidget->show();
}

void
CallWidget::on_muteSpeakerButton_toggled(bool checked)
{
    Audio::Settings::instance()->mutePlayback(checked);
}

void
CallWidget::on_muteMicButton_toggled(bool checked)
{
    Audio::Settings::instance()->muteCapture(checked);
}

void
CallWidget::on_speakerSlider_sliderMoved(int position)
{
    outputVolume_ = position;
}

void
CallWidget::on_speakerSlider_sliderReleased()
{
    emit Audio::Settings::instance()->setPlaybackVolume(outputVolume_);
}

void
CallWidget::on_micSlider_sliderMoved(int position)
{
    inputVolume_ = position;
}

void
CallWidget::on_micSlider_sliderReleased()
{
    emit Audio::Settings::instance()->setCaptureVolume(inputVolume_);
}

void
CallWidget::atExit() {

}
