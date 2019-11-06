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
}

RecordOverlay::~RecordOverlay()
{
    delete ui;
}

void
RecordOverlay::setUpRecorderStatus(RecorderStatus status)
{
    switch(status) {
    case RecorderStatus::aboutToRecord:
        switchToAboutToRecordPage();
        break;
    case RecorderStatus::recording:
        switchToRecordingPage();
        break;
    case RecorderStatus::recorded:
        switchToRecordedPage();
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
RecordOverlay::on_recordOverlayStartOrFinishRecordingBtn_toggled(bool checked)
{
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
        qDebug() << "The contented is not recorded and cannot be deleted";
        return;
    }
    if(recordWidget_->actionToDeleteRecord()) {
        setUpRecorderStatus(RecorderStatus::aboutToRecord);
    }
}

void
RecordOverlay::on_recordOverlayRerecordBtn_pressed()
{
}

void
RecordOverlay::whenAudioRecordProgressUpdate()
{
    currentTime_ ++;
    ui->timerLabel->setText(Utils::convertTimeDisplayFromMilisecond(currentTime_));
}
