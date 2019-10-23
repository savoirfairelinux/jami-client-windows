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
#include "../misc/utils.h"

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

    setUpRecorderStatus(RecorderStatus::aboutToRecord);
}

RecordOverlay::~RecordOverlay()
{
    delete ui;
}

void
RecordOverlay::setUpRecorderStatus(RecorderStatus status, bool isTimerToBeInvolved, bool isAimationToBeInvolved)
{
    status_ = status;
    switch(status) {
    case RecorderStatus::aboutToRecord:
        switchToAboutToRecordPage();
        if(isTimerToBeInvolved) {reinitializeTimer();}
        if(isAimationToBeInvolved) {stopRedDotBlink();}
        break;
    case RecorderStatus::recording:
        switchToRecordingPage();
        if(isTimerToBeInvolved) {
            reinitializeTimer();
            recordTimer_.start(1000);
        }
        if(isAimationToBeInvolved) {
            stopRedDotBlink();
            startRedDotBlink();
        }
        break;
    case RecorderStatus::recorded:
        switchToRecordedPage();
        if(isTimerToBeInvolved) {reinitializeTimer();}
        if(isAimationToBeInvolved) {stopRedDotBlink();}
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
RecordOverlay::startRedDotBlink()
{
    ui->redDotBlinkable->start();
}

void
RecordOverlay::stopRedDotBlink()
{
    ui->redDotBlinkable->stop();
}

void
RecordOverlay::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    setUpRecorderStatus(RecorderStatus::aboutToRecord);
}

void
RecordOverlay::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    // set the page to about record page
    setUpRecorderStatus(RecorderStatus::aboutToRecord);
}

void
RecordOverlay::on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked)
{
    if(status_ == RecorderStatus::aboutToRecord) {
        // start record function call, if call succeed, switch the page to recording page and start the timer
        if(recordWidget_->startRecording()) {
            setUpRecorderStatus(RecorderStatus::recording);
        } else {
            ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(!checked);
            qDebug() << "The recording does not start properly";
        }
    } else if(status_ == RecorderStatus::recording) {
        // finish the record, if it succeed, switch the page and re-initialize the timer
        if(recordWidget_->finishRecording()) {
            setUpRecorderStatus(RecorderStatus::recorded);
        } else {
            ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(!checked);
            qDebug() << "The recording does not finish properly";
        }
    } else {

    }
}

void
RecordOverlay::on_recordOverlaySendBtn_pressed()
{
    if(status_ != RecorderStatus::recorded) {
        qDebug() << "The contented is not recorded and cannot be sent out";
        return;
    }
    if(recordWidget_->sendRecording()) {
        setUpRecorderStatus(RecorderStatus::aboutToRecord);
        // define what to do when the record is sent out
        recordWidget_->getContainer()->accept();
    }
}

void
RecordOverlay::on_recordOverlayDeleteBtn_pressed()
{
    if(status_ != RecorderStatus::recorded) {
        qDebug() << "The content is not recorded and cannot be deleted";
        return;
    }
    recordWidget_->deleteRecording();
    setUpRecorderStatus(RecorderStatus::aboutToRecord);
}

void
RecordOverlay::on_recordOverlayRerecordBtn_pressed()
{
    if(status_ != RecorderStatus::recording) {
        qDebug() << "it's not on the recording page and thus there's cannot re-record";
        return;
    }
    recordWidget_->recordAgain();
    // re-initialize the timer at the same time
    setUpRecorderStatus(RecorderStatus::recording);
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
