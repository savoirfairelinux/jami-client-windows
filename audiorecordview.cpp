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

    QFont font = ui->timeDisplay->font();
    font.setPointSize(40);
    font.setBold(true);
    ui->timeDisplay->setFont(font);
    recordTimer_ = new QTimer(this);
    connect(recordTimer_,SIGNAL(timeout()),this,SLOT(on_audioRecordProgressUpdate()));
    setUpStatus(AudioRecorderStatus::aboutToRecord);
}

AudioRecordView::~AudioRecordView()
{
    delete audioPlayer_;
    delete ui;
}

bool
AudioRecordView::getDeleteFileWhenInterrupt()
{
    return deleteFileWhenInterrupt_;
}

void
AudioRecordView::setDeleteFileWhenInterrupt(bool value)
{
    deleteFileWhenInterrupt_ = value;
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
#ifdef AUDIO_PLAYER_NOT_READY
    ui->playBtn->setVisible(false);
#endif
}

void
AudioRecordView::initializeOverlayBtnCheckState(QPushButton* btn, bool isToCheck)
{
    if(OverlayButton* overlaybtn = (OverlayButton*)btn)
    {
        try
        {
            overlaybtn->setOverlayButtonChecked(isToCheck);
        }
        catch (...)
        {
            qDebug() << "AudioRecordView: Overlay button cast fail exception!";
        }
    }
}

void
AudioRecordView::deleteAudioRecord()
{
    if(status_ == AudioRecorderStatus::recording)
    {
        LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
        recordTimer_->stop();
        currentTime_ = 0;
    }
    try
    {
        QFile file(recordedAudioFilePath_);
        file.remove();
    }
    catch (...)
    {
        qDebug() << "Audio Record View: delete file fails!";
    }
    recordTimer_->stop();
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordedAudioFilePath_ = "";
}

void
AudioRecordView::switchToAboutToRecordPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("Start Record");
    ui->startAndFinishRecordBtn->setVisible(true);
    if(ui->startAndFinishRecordBtn->isChecked()) initializeOverlayBtnCheckState(ui->startAndFinishRecordBtn,false);
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
    ui->startAndFinishRecordBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->stopAndDeleteBtn,false);
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(true);
    ui->sendBtn->setVisible(false);
    ui->stopPlayingBtn->setVisible(false);

    if(!ui->startAndFinishRecordBtn->isChecked()) initializeOverlayBtnCheckState(ui->startAndFinishRecordBtn,true);

    ui->startAndFinishRecordBtn->setToolTip("press to finish the audio record");
    ui->stopAndDeleteBtn->setToolTip("press to stop the record");
    ui->reRecordBtn->setToolTip("press to restart the audio record");
}

void
AudioRecordView::switchToRecordedPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("Completed");
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->playBtn,false);
    ui->reRecordBtn->setVisible(true);
    ui->sendBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(false);

    ui->playBtn->setToolTip("press to play the recorded clip");
    ui->reRecordBtn->setToolTip("press to restart the record");
    ui->sendBtn->setToolTip("press to send");
    ui->stopAndDeleteBtn->setToolTip("press to delete the recorded clip");
}

void
AudioRecordView::switchToPlayingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("00:00");
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    if(!ui->playBtn->isChecked()) initializeOverlayBtnCheckState(ui->playBtn,true);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(true);

    ui->playBtn->setToolTip("press to pause");
    ui->stopAndDeleteBtn->setToolTip("press to delete the recorded clip");
    ui->stopPlayingBtn->setToolTip("Stop the play");
}

void
AudioRecordView::startAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recording);
    recordedAudioFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(true));
    currentTime_ = 0;
    recordTimer_->start(1000);
}

void
AudioRecordView::finishAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recorded);
    LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
    recordTimer_->stop();
}

void
AudioRecordView::interruptAudioRecord(bool willDeleteFile)
{
    if(status_ == AudioRecorderStatus::recording) {
        recordTimer_->stop();
        if(willDeleteFile) {
        deleteAudioRecord();
        } else {
            LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
            setUpStatus(AudioRecorderStatus::aboutToRecord);
        }
    }
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
    if(audioPlayer_->isAudioAvailable())
    {
        // play the audio
        audioPlayer_->play();
    }
    else
    {
        qDebug() << "AudioRecorderView: The audio file is not playable, audio path: " + recordedAudioFilePath_;
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
    // delete current audio file and restart recording
    deleteAudioRecord();
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
    if(status_ == AudioRecorderStatus::recording) {
        interruptAudioRecord(deleteFileWhenInterrupt_);
    } else {
        deleteAudioRecord();
    }
    emit recordControllerShouldCloseSignal();
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
    if(status_ != AudioRecorderStatus::playing)
    {
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
    deleteAudioRecord();
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordedAudioFilePath_ = "";
    recordTimer_->stop();
    currentTime_ = 0;
}

void
AudioRecordView::on_audioPlayFinish(QMediaPlayer::MediaStatus status)
{
    if(status != QMediaPlayer::EndOfMedia)
        return;
    setUpStatus(AudioRecorderStatus::recorded);
    // set the play button back to unchecked
    initializeOverlayBtnCheckState(ui->playBtn,false);
}

void
AudioRecordView::on_audioPlayProgressUpdate(qint64 position)
{
    // update the time display in the time display label the playing progress
    QString timeToDisplay = convertTimeDisplayFromMilisecond(position) + " / " + convertTimeDisplayFromMilisecond(audioPlayer_->duration());
    ui->timeDisplay->setText(timeToDisplay);
}

void
AudioRecordView::on_audioRecordProgressUpdate()
{
    // update the time display label the recording time
    currentTime_ ++;
    ui->timeDisplay->setText(convertTimeDisplayFromMilisecond(currentTime_ * 1000));
}

QString AudioRecordView::convertTimeDisplayFromMilisecond(qint64 milisecond)
{
    int seconds = (int)(milisecond / 1000);
    int minutes = seconds/ 60;
    int secondToDisplay = seconds % 60;
    std::string sec, min, time;
    if(secondToDisplay / 10 < 1)
    {
        sec = "0" + std::to_string(secondToDisplay);
    }else
    {
        sec = std::to_string(secondToDisplay);
    }
    if(minutes / 10 < 1)
    {
        min = "0" + std::to_string(minutes);
    }else
    {
        min = std::to_string(minutes);
    }
    time = min + ":" + sec;

    return QString::fromStdString(time);
}

void
AudioRecordView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    // define what to do when the audio recorder shows
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordedAudioFilePath_ = "";
    currentTime_= 0;
    recordTimer_->stop();
}

void
AudioRecordView::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    // define what to do when the audio recorder close
    on_audioRecordViewClose();
}
