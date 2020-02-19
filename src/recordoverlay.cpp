/**************************************************************************
* Copyright (C) 2019 by Savoir-faire Linux                                *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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

#include "recordoverlay.h"
#include "ui_recordoverlay.h"

#include "recordwidget.h"
#include "lrcinstance.h"
#include "utils.h"

RecordOverlay::RecordOverlay(RecordWidget* recordWidget) :
    VignetteWidget(qobject_cast<QWidget*>(recordWidget)),
    ui(new Ui::RecordOverlay)
{
    ui->setupUi(this);

    recordWidget_ = recordWidget;

    QFont font = ui->timerLabel->font();
    font.setPointSize(10);
    font.setBold(true);
    ui->timerLabel->setFont(font);

    connect(&recordTimer_, SIGNAL(timeout()),SLOT(whenAudioRecordProgressUpdate()));
    ui->redDotBlinkable->setPixmap(&redDotPix_);
    ui->recordOverlayPlayBtn->setVisible(false);
    ui->recordOverlayStopPlayingBtn->setVisible(false);

    connect(&LRCInstance::avModel(), &lrc::api::AVModel::audioMeter,
        ui->levelMeter,
        [this](const QString& /*id*/, float level) {
            ui->levelMeter->setLevel(level);
        });

    ui->inputIcon->setVisible(false);

    ui->inputIcon->setPixmap(Utils::pixmapFromSvg(
        QString(":/images/icons/av_icons/mic-24px.svg"),
        ui->inputIcon->size()));

    setRecorderState(RecorderState::aboutToRecord);
}

RecordOverlay::~RecordOverlay()
{
    delete ui;
}

void
RecordOverlay::setRecorderState(RecorderState state)
{
    reinitializeTimer();
    recorderState_ = state;
    switch(recorderState_) {
    case RecorderState::aboutToRecord:
        switchToAboutToRecordPage();
        ui->redDotBlinkable->stop();
        break;
    case RecorderState::recording:
        switchToRecordingPage();
        recordTimer_.start(1000);
        ui->redDotBlinkable->stop();
        ui->redDotBlinkable->start();
        break;
    case RecorderState::recorded:
        switchToRecordedPage();
        ui->redDotBlinkable->stop();
        break;
    }
}

void
RecordOverlay::switchToAboutToRecordPage()
{
    ui->timerLabel->setVisible(true);
    ui->timerLabel->setText("00:00");
    ui->redDotBlinkable->setVisible(false);
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(true);
    if (ui->recordOverlayStartOrFinishRecordingBtn->isChecked()) {
        ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(false);
    }
    ui->recordOverlayRerecordBtn->setVisible(false);
    ui->recordOverlaySendBtn->setVisible(false);
    ui->recordOverlayDeleteBtn->setVisible(false);
    ui->recordOverlayStartOrFinishRecordingBtn->setToolTip(tr("press to start the audio record"));
}

void
RecordOverlay::switchToRecordingPage()
{
    ui->timerLabel->setVisible(true);
    ui->timerLabel->setText("00:00");
    ui->redDotBlinkable->setVisible(true);
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(true);
    ui->recordOverlayDeleteBtn->setVisible(false);
    ui->recordOverlayRerecordBtn->setVisible(true);
    ui->recordOverlaySendBtn->setVisible(false);
    if (!ui->recordOverlayStartOrFinishRecordingBtn->isChecked()) {
        ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(true);
    }
    // set the tool tip
    ui->recordOverlayStartOrFinishRecordingBtn->setToolTip(tr("press to finish the audio record"));
    ui->recordOverlayDeleteBtn->setToolTip(tr("press to stop the record"));
    ui->recordOverlayRerecordBtn->setToolTip(tr("press to restart the audio record"));
}

void
RecordOverlay::switchToRecordedPage()
{
    ui->redDotBlinkable->setVisible(false);
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(false);
    ui->recordOverlayRerecordBtn->setVisible(false);
    ui->recordOverlaySendBtn->setVisible(true);
    ui->recordOverlayDeleteBtn->setVisible(true);
    // set the tool tip
    ui->recordOverlayRerecordBtn->setToolTip(tr("press to restart the record"));
    ui->recordOverlaySendBtn->setToolTip(tr("press to send"));
    ui->recordOverlayDeleteBtn->setToolTip(tr("press to delete the recorded clip"));
}

void
RecordOverlay::reinitializeTimer()
{
    recordTimer_.stop();
    currentTime_ = 0;
}

void
RecordOverlay::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if(recordWidget_->isAudio()) {
        ui->inputIcon->setVisible(true);
        ui->levelMeter->setVisible(true);
        ui->levelMeter->start();
        LRCInstance::startAudioMeter(true);
    } else {
        ui->inputIcon->setVisible(false);
        ui->levelMeter->setVisible(false);
        ui->levelMeter->stop();
        LRCInstance::stopAudioMeter(true);
    }
    setRecorderState(RecorderState::aboutToRecord);
}

void
RecordOverlay::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    if(recordWidget_->isAudio()) {
        ui->levelMeter->stop();
        ui->levelMeter->setVisible(false);
        ui->inputIcon->setVisible(false);
        LRCInstance::stopAudioMeter(true);
    }
    // set the page to about record page
    setRecorderState(RecorderState::aboutToRecord);
}

void
RecordOverlay::on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked)
{
    if(recorderState_ == RecorderState::aboutToRecord) {
        // start record function call, if call succeed, switch the page to recording page and start the timer
        if(recordWidget_->startRecording()) {
            setRecorderState(RecorderState::recording);
        } else {
            ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(!checked);
            qDebug() << "The recording does not start properly";
        }
    } else if(recorderState_ == RecorderState::recording) {
        // finish the record, if it succeed, switch the page and re-initialize the timer
        if(recordWidget_->finishRecording()) {
            setRecorderState(RecorderState::recorded);
        } else {
            ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(!checked);
            qDebug() << "The recording does not finish properly";
        }
    } else {
        ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(!checked);
        qDebug() << "This button should not appear on current page";
    }
}

void
RecordOverlay::on_recordOverlaySendBtn_pressed()
{
    if(recorderState_ != RecorderState::recorded) {
        qDebug() << "The contented is not recorded and cannot be sent out";
        return;
    }
    if(recordWidget_->sendRecording()) {
        setRecorderState(RecorderState::aboutToRecord);
        // define what to do when the record is sent out
        if (auto container = recordWidget_->getContainer().toStrongRef()) {
            container->accept();
        }
    }
}

void
RecordOverlay::on_recordOverlayDeleteBtn_pressed()
{
    if(recorderState_ != RecorderState::recorded) {
        qDebug() << "The content is not recorded and cannot be deleted";
        return;
    }
    recordWidget_->deleteRecording();
    setRecorderState(RecorderState::aboutToRecord);
}

void
RecordOverlay::on_recordOverlayRerecordBtn_pressed()
{
    if(recorderState_ != RecorderState::recording) {
        qDebug() << "it's not on the recording page and thus there's cannot re-record";
        return;
    }
    recordWidget_->recordAgain();
    // re-initialize the timer at the same time
    setRecorderState(RecorderState::recording);
}

void
RecordOverlay::whenAudioRecordProgressUpdate()
{
    ui->timerLabel->setText(Utils::formattedTime(++currentTime_));
}

QPixmap
RecordOverlay::getOriginPix()
{
    return redDotPix_;
}

void
RecordOverlay::setOriginPix(QPixmap pix)
{
    redDotPix_ = pix;
}
