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
    audioPlayer_(new QMediaPlayer()),
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
    connect(&recordTimer_,SIGNAL(timeout()),this,SLOT(on_audioRecordProgressUpdate()));

    //initialize the record page
    setUpStatus(AudioRecorderStatus::aboutToRecord);
}

AudioRecordView::~AudioRecordView()
{
    delete audioPlayer_;
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
    case AudioRecorderStatus::playing:
        switchToPlayingPage();
        break;;
    }

    // TODO: Delete this line in the future when audio player is in place
    ui->playBtn->setVisible(false);
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
                while (!file.remove()) {
                    qDebug().noquote() << "\n" << file.errorString() << "\n";
                    if (!QFile::exists(deleteFilePath))
                        break;
                    QThread::msleep(10);
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
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(false);
    ui->stopPlayingBtn->setVisible(false);
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
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(true);
    ui->sendBtn->setVisible(false);
    ui->stopPlayingBtn->setVisible(false);
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
    ui->playBtn->setVisible(true);
    ui->playBtn->setOverlayButtonChecked(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(false);
    // set the tool tip
    ui->playBtn->setToolTip("press to play the recorded clip");
    ui->reRecordBtn->setToolTip("press to restart the record");
    ui->sendBtn->setToolTip("press to send");
    ui->stopAndDeleteBtn->setToolTip("press to delete the recorded clip");
}

void
AudioRecordView::switchToPlayingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->redDot->setVisible(false);
    ui->timeDisplay->setText("00:00");
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    if(!ui->playBtn->isChecked()) ui->playBtn->setOverlayButtonChecked(true);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(true);
    // set the tool tip
    ui->playBtn->setToolTip("press to pause");
    ui->stopAndDeleteBtn->setToolTip("press to delete the recorded clip");
    ui->stopPlayingBtn->setToolTip("Stop the play");
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
AudioRecordView::playAudio()
{
    // set the ui to play audio page
    setUpStatus(AudioRecorderStatus::playing);
    QObject::connect(audioPlayer_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_audioPlayFinish(QMediaPlayer::MediaStatus)));
    QObject::connect(audioPlayer_, SIGNAL(positionChanged(qint64)),this,SLOT(on_audioPlayProgressUpdate(qint64)));
    audioPlayer_->setMedia(QMediaContent(QUrl::fromLocalFile(recordedAudioFilePath_)));
    audioPlayer_->setNotifyInterval(100);
    audioPlayer_->setVolume(50);
    audioPlayer_->play();
    if(audioPlayer_->isAudioAvailable()) {
        // play the audio
        audioPlayer_->play();
    } else {
        qDebug() << "AudioRecorderView: The audio file is not playable, audio path: " + recordedAudioFilePath_;
    }
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
        switchToRecordingPage();
    } catch (...) {
        qDebug() << "Audio controller is sending out the audio file!";
    }
    // close the record controller
    emit recordControllerShouldCloseSignal();
}

void
AudioRecordView::on_reRecordBtn_pressed()
{
    // finish current audio file and restart recording
    finishAudioRecord();
    startAudioRecord();
}

void
AudioRecordView::on_startAndFinishRecordBtn_toggled(bool checked)
{
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
AudioRecordView::on_playBtn_toggled(bool checked)
{
    if(checked) {
        playAudio();
    } else {
        try
        {
            audioPlayer_->pause();
        }
        catch (...)
        {
            qDebug() << "AudioRecordView: trying to pause the audio fail";
        }
    }
}

void
AudioRecordView::on_stopPlayingBtn_pressed()
{
    if(status_ != AudioRecorderStatus::playing) {
        return;
    }
    // stop the ongoing audio
    audioPlayer_->stop();
    // switch the UI to "recorded" state
    setUpStatus(AudioRecorderStatus::recorded);
}

void
AudioRecordView::on_audioRecordViewClose()
{
    if(status_ != AudioRecorderStatus::recorded)
    deleteAudioRecord();
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordTimer_.stop();
    currentTime_ = 0;
}

void
AudioRecordView::on_audioPlayFinish(QMediaPlayer::MediaStatus status)
{
    if(status != QMediaPlayer::EndOfMedia)
        return;
    setUpStatus(AudioRecorderStatus::recorded);
    // set the play button back to unchecked
    ui->playBtn->setOverlayButtonChecked(false);
}

void
AudioRecordView::on_audioPlayProgressUpdate(qint64 position)
{
    // update the time display in the time display label the playing progress
    QString timeToDisplay = Utils::convertTimeDisplayFromMilisecond(position / 1000) + " / " + Utils::convertTimeDisplayFromMilisecond(audioPlayer_->duration() / 1000);
    ui->timeDisplay->setText(timeToDisplay);
}

void
AudioRecordView::on_audioRecordProgressUpdate()
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
    on_audioRecordViewClose();
    redDotAnimTimer_.stop();
}
