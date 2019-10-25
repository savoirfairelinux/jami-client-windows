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

    recordTimer = new QTimer(this);
    connect(recordTimer,SIGNAL(timeout()),this,SLOT(on_audioRecordProgressUpdate()));
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
        recordTimer->stop();
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
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    recordedAudioFilePath_ = "";
}

void
AudioRecordView::switchToAboutToRecordPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("");
    ui->startAndFinishRecordBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->startAndFinishRecordBtn,false);
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(false);
    ui->stopPlayingBtn->setVisible(false);
}

void
AudioRecordView::switchToRecordingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->startAndFinishRecordBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->startAndFinishRecordBtn,false);
    initializeOverlayBtnCheckState(ui->stopAndDeleteBtn,false);
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopPlayingBtn->setVisible(false);
}

void
AudioRecordView::switchToRecordedPage()
{
    ui->timeDisplay->setVisible(true);
    ui->timeDisplay->setText("");
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->playBtn,false);
    ui->reRecordBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->reRecordBtn,false);
    ui->sendBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->sendBtn,false);
    ui->stopAndDeleteBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->stopAndDeleteBtn,false);
    ui->stopPlayingBtn->setVisible(false);
}

void
AudioRecordView::switchToPlayingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->playBtn,false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->stopAndDeleteBtn,false);
    ui->stopPlayingBtn->setVisible(true);
    initializeOverlayBtnCheckState(ui->stopPlayingBtn,false);
}

void
AudioRecordView::startAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recording);
    recordedAudioFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(true));
    currentTime_ = 0;
    recordTimer->start(1000);
}

void
AudioRecordView::finishAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recorded);
    LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
    recordTimer->stop();
}

void
AudioRecordView::interruptAudioRecord(bool willDeleteFile)
{
    if(status_ == AudioRecorderStatus::recording) {
        recordTimer->stop();
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
        //TODO: close the record controller

    } catch (...) {
        qDebug() << "Audio controller is sending out the audio file!";
    }
    emit recordControllerShouldCloseSignal();
}

void
AudioRecordView::on_reRecordBtn_pressed()
{
    if(status_ != AudioRecorderStatus::recorded)
        return;
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
        /*if(checked)
        {
            Utils::whileBlocking(ui->startAndFinishRecordBtn)->setChecked(!checked);
            return;
        }*/
        startAudioRecord();
        break;
    case AudioRecorderStatus::recording:
        /*if(!checked)
        {
            Utils::whileBlocking(ui->startAndFinishRecordBtn)->setChecked(!checked);
            return;
        }*/
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

void AudioRecordView::on_audioRecordProgressUpdate()
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
