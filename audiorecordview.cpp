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

// LRC
#include "lrcinstance.h"

//Qt
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QtConcurrent/QtConcurrent>

// Some module
#include "audiorecordview.h"
#include "ui_audiorecordview.h"

#include "utils.h"

AudioRecordView::AudioRecordView(QWidget *parent) :
    QWidget(parent),
    recordedAudioFilePath_(""),
    currentTime_(0),
    ui(new Ui::AudioRecordView)
{
    ui->setupUi(this);

    // set up the animation property for the red dot when recording is going on
    auto effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(maxOverlayOpacity_);
    ui->redDot->setGraphicsEffect(effect);
    redDotFadeAnim_ = new QPropertyAnimation(this);
    redDotFadeAnim_->setTargetObject(effect);
    redDotFadeAnim_->setPropertyName("opacity");
    redDotFadeAnim_->setDuration(fadeRedDotTime_);
    redDotFadeAnim_->setEndValue(0);
    redDotFadeAnim_->setStartValue(effect->opacity());
    redDotFadeAnim_->setEasingCurve(QEasingCurve::OutQuad);
    connect(&redDotAnimTimer_,SIGNAL(timeout()),this,SLOT(onRedDotShouldToggle()));

    //set up the font for the timer display
    QFont font = ui->timeDisplay->font();
    font.setPointSize(20);
    font.setBold(true);
    ui->timeDisplay->setFont(font);

    // set up the timer for the time display when recording
    connect(&recordTimer_,SIGNAL(timeout()),this,SLOT(whenAudioRecordProgressUpdate()));

    //initialize the record page
    setUpStatus(AudioRecorderStatus::aboutToRecord);
}

AudioRecordView::~AudioRecordView()
{
    delete redDotFadeAnim_;
    delete ui;
}

void
AudioRecordView::setUpStatus(AudioRecorderStatus status)
{
    status_ = status;
    switch(status)
    {
    case AudioRecorderStatus::aboutToRecord:
        switchToAboutToRecordPage();
        break;
    case AudioRecorderStatus::recording:
        switchToRecordingPage();
        break;;
    case AudioRecorderStatus::recorded:
        switchToRecordedPage();
        break;;
    }
}

void
AudioRecordView::deleteAudioRecord()
{
    if(status_ == AudioRecorderStatus::recording) {
        LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
        recordTimer_.stop();
        currentTime_ = 0;
    }
    try {
        QString deleteFilePath = recordedAudioFilePath_;
        // keep deleting file until the process holding it let go
        QtConcurrent::run(
        [deleteFilePath] {
            QFile file(deleteFilePath);
            if (!QFile::exists(deleteFilePath))
                return;
            int deletionCount { 0 };
            while (!file.remove() && deletionCount < 5) {
                qDebug().noquote() << "\n" << file.errorString() << "\n";
                QThread::msleep(10);
                ++ deletionCount;
            }
        });
    }
    catch (...) {
        qDebug() << "Audio Record View: delete file fails!";
    }
    recordTimer_.stop();
    redDotAnimTimer_.stop();
    setUpStatus(AudioRecorderStatus::aboutToRecord);
}

void
AudioRecordView::switchToAboutToRecordPage()
{
    ui->timeDisplay->setVisible(true);
    ui->redDot->setVisible(false);
    ui->redDot->setVisible(false);
    ui->timeDisplay->setText("Start Record");
    ui->startAndFinishRecordBtn->setVisible(true);
    if(ui->startAndFinishRecordBtn->isChecked()) ui->startAndFinishRecordBtn->setOverlayButtonChecked(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(false);
    ui->startAndFinishRecordBtn->setToolTip("press to start the audio record");
}

void
AudioRecordView::switchToRecordingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("00:00");
    ui->redDot->setVisible(true);
    ui->startAndFinishRecordBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(false);
    ui->reRecordBtn->setVisible(true);
    ui->sendBtn->setVisible(false);
    if(!ui->startAndFinishRecordBtn->isChecked()) ui->startAndFinishRecordBtn->setOverlayButtonChecked(true);
    // set the tool tip
    ui->startAndFinishRecordBtn->setToolTip("press to finish the audio record");
    ui->stopAndDeleteBtn->setToolTip("press to stop the record");
    ui->reRecordBtn->setToolTip("press to restart the audio record");
}

void
AudioRecordView::switchToRecordedPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("Completed");
    ui->redDot->setVisible(false);
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    // set the tool tip
    ui->reRecordBtn->setToolTip("press to restart the record");
    ui->sendBtn->setToolTip("press to send");
    ui->stopAndDeleteBtn->setToolTip("press to delete the recorded clip");
}

void
AudioRecordView::startAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recording);
    redDotAnimTimer_.start(startfadeRedDotTime_);
    recordedAudioFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(true));
    currentTime_ = 0;
    recordTimer_.start(1000);
}

void
AudioRecordView::finishAudioRecord()
{
    redDotAnimTimer_.stop();
    setUpStatus(AudioRecorderStatus::recorded);
    LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
    recordTimer_.stop();
}

void
AudioRecordView::showRedDot()
{
    redDotFadeAnim_->stop();
    redDotFadeAnim_->targetObject()->setProperty(redDotFadeAnim_->propertyName(),
                                           redDotFadeAnim_->startValue());
}

void
AudioRecordView::fadeRedDotOut()
{
    redDotFadeAnim_->start(QAbstractAnimation::KeepWhenStopped);
}

void
AudioRecordView::onRedDotShouldToggle()
{
    if(shouldFadeRedDot_) {
        fadeRedDotOut();
        shouldFadeRedDot_ = false;
    } else {
        showRedDot();
        shouldFadeRedDot_ = true;
    }
}

void
AudioRecordView::on_sendBtn_pressed()
{
    if(status_ != AudioRecorderStatus::recorded)
        return;
    // send out the stored audio file
    QFileInfo fi(recordedAudioFilePath_);
    QString fileName = fi.fileName();
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendFile(convUid, recordedAudioFilePath_.toStdString(), fileName.toStdString());
        //switch widget
    } catch (...) {
        qDebug() << "Audio controller is sending out the audio file!";
    }
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    // close the record controller
    emit recordControllerShouldCloseSignal();
}

void
AudioRecordView::on_reRecordBtn_pressed()
{
    // finish current audio file and restart recording
    deleteAudioRecord();
    startAudioRecord();
}

void
AudioRecordView::on_startAndFinishRecordBtn_toggled(bool checked)
{
    Q_UNUSED(checked)

    switch(status_)
    {
    case AudioRecorderStatus::aboutToRecord:
        startAudioRecord();
        break;
    case AudioRecorderStatus::recording:
        finishAudioRecord();
        break;;
    default:
        break;
    }
}

void
AudioRecordView::on_stopAndDeleteBtn_pressed()
{
    deleteAudioRecord();
}

void
AudioRecordView::whenAudioRecordViewClose()
{
    if(status_ == AudioRecorderStatus::recording) {
        deleteAudioRecord();
    }
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordTimer_.stop();
    currentTime_ = 0;
    redDotAnimTimer_.stop();
}

void
AudioRecordView::whenAudioRecordProgressUpdate()
{
    // update the time display label the recording time
    currentTime_ ++;
    ui->timeDisplay->setText(Utils::convertTimeDisplayFromMilisecond(currentTime_));
}

void
AudioRecordView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    // define what to do when the audio recorder shows
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    currentTime_= 0;
    recordTimer_.stop();
    redDotAnimTimer_.stop();
}

void
AudioRecordView::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    // define what to do when the audio recorder close
    whenAudioRecordViewClose();
}
