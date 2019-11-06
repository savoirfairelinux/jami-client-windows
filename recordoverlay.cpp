/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
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

#include "recordwidget.h"

#include "recordoverlay.h"
#include "ui_recordoverlay.h"

#include "utils.h"

RecordOverlay::RecordOverlay(RecordWidget* popupWidget) :
    VignetteWidget(qobject_cast<QWidget*>(popupWidget)),
    ui(new Ui::RecordOverlay)
{
    ui->setupUi(this);

    QFont font = ui->timerLabel->font();
    font.setPointSize(10);
    font.setBold(true);
    ui->timerLabel->setFont(font);

    connect(&recordTimer_, SIGNAL(timeout()),SLOT(whenAudioRecordProgressUpdate()));
    ui->redDotBlinkable->setPixmap(&redDotPix_);
}

RecordOverlay::~RecordOverlay()
{
    delete ui;
}

void
RecordOverlay::setUpRecorderStatus(RecorderStatus status, bool isTimerToBeInvolved, bool isAimationToBeInvolved)
{
    switch(status) {
    case RecorderStatus::aboutToRecord:
        switchToAboutToRecordPage();
        if(isTimerToBeInvolved) {reinitializeTimer();}
        if(isAimationToBeInvolved) {stopRedDotBlink();}
        break;
    case RecorderStatus::recording:
        switchToRecordingPage();
        if(isTimerToBeInvolved) {recordTimer_.start(1000);}
        if(isAimationToBeInvolved) {startRedDotBlink();}
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
    ui->timerLabel->setText("Start Record");
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(true);
    if(ui->recordOverlayStartOrFinishRecordingBtn->isChecked()) ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(false);
    ui->recordOverlayRerecordBtn->setVisible(false);
    ui->recordOverlaySendBtn->setVisible(false);
    ui->recordOverlayDeleteBtn->setVisible(false);
    ui->recordOverlayStartOrFinishRecordingBtn->setToolTip("press to start the audio record");
}

void
RecordOverlay::switchToRecordingPage()
{
    ui->timerLabel->setVisible(true);
    ui->timerLabel->setText("00:00");
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(true);
    ui->recordOverlayDeleteBtn->setVisible(false);
    ui->recordOverlayRerecordBtn->setVisible(true);
    ui->recordOverlaySendBtn->setVisible(false);
    if(!ui->recordOverlayStartOrFinishRecordingBtn->isChecked()) ui->recordOverlayStartOrFinishRecordingBtn->setOverlayButtonChecked(true);
    // set the tool tip
    ui->recordOverlayStartOrFinishRecordingBtn->setToolTip("press to finish the audio record");
    ui->recordOverlayDeleteBtn->setToolTip("press to stop the record");
    ui->recordOverlayRerecordBtn->setToolTip("press to restart the audio record");
}

void
RecordOverlay::switchToRecordedPage()
{
    ui->timerLabel->setVisible(true);
    ui->timerLabel->setText("Completed");
    ui->recordOverlayStartOrFinishRecordingBtn->setVisible(false);
    ui->recordOverlayRerecordBtn->setVisible(false);
    ui->recordOverlaySendBtn->setVisible(true);
    ui->recordOverlayDeleteBtn->setVisible(true);
    // set the tool tip
    ui->recordOverlayRerecordBtn->setToolTip("press to restart the record");
    ui->recordOverlaySendBtn->setToolTip("press to send");
    ui->recordOverlayDeleteBtn->setToolTip("press to delete the recorded clip");
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
    // set the page to about record page
    setUpRecorderStatus(RecorderStatus::aboutToRecord);
    // re-initialize the timer
    reinitializeTimer();
    // TODO: re-initialize the red dot's animation
}

void
RecordOverlay::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    // set the page to about record page
    setUpRecorderStatus(RecorderStatus::aboutToRecord);
    // re-initialize the timer
    reinitializeTimer();
    // TODO: re-initialize the red dot's animation
}

void
RecordOverlay::on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked)
{
    Q_UNUSED(checked)
    if(status_ == RecorderStatus::aboutToRecord) {
        // start record function call, if call succeed, switch the page to recording page and start the timer
        if(recordWidget_->actionToStartRecord()) {
            setUpRecorderStatus(RecorderStatus::recording);
            //TODO: start the timer

        }
    } else if(status_ == RecorderStatus::recording) {
        // finish the record, if it succeed, switch the page and re-initialize the timer
        if(recordWidget_->actionToFinishRecord()) {
            setUpRecorderStatus(RecorderStatus::recorded);
            // TODO: turn off and re-initialize the timer

        }
    }
}

void
RecordOverlay::on_recordOverlaySendBtn_pressed()
{
    if(status_ != RecorderStatus::recorded) {
        qDebug() << "The contented is not recorded and cannot be sent out";
        return;
    }
    if(recordWidget_->actionToSend()) {
        setUpRecorderStatus(RecorderStatus::aboutToRecord);
        // initialize the timer
        // turn off the whole view?
    }
    return;
}

void
RecordOverlay::on_recordOverlayDeleteBtn_pressed()
{
    if(status_ != RecorderStatus::recorded) {
        qDebug() << "The contente is not recorded and cannot be deleted";
        return;
    }
    if(recordWidget_->actionToDeleteRecord()) {
        setUpRecorderStatus(RecorderStatus::aboutToRecord);
    }
}

void
RecordOverlay::on_recordOverlayRerecordBtn_pressed()
{
    if(status_ != RecorderStatus::recording) {
        qDebug() << "it's not on the recording page and thus there's cannot re-record";
        return;
    }
    if(recordWidget_->actionToReRecord()) {
        // re-initialize the timer at the same time
        setUpRecorderStatus(RecorderStatus::recording);
    }
}

void
RecordOverlay::whenAudioRecordProgressUpdate()
{
    currentTime_ ++;
    ui->timerLabel->setText(Utils::convertTimeDisplayFromMilisecond(currentTime_));
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

