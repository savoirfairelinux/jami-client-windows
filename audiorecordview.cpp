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

// Same module
#include "audiorecordview.h"
#include "ui_audiorecordview.h"

AudioRecordView::AudioRecordView(QWidget *parent) :
    QWidget(parent),
    recordedAudioFilePath_(""),
    ui(new Ui::AudioRecordView)
{
    ui->setupUi(this);

    setUpStatus(AudioRecorderStatus::aboutToRecord);
}

AudioRecordView::~AudioRecordView()
{
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
}

void
AudioRecordView::deleteAudioRecord()
{
    if(status_ == AudioRecorderStatus::aboutToRecord)
        return;
    if(status_ == AudioRecorderStatus::recording)
    {
        LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
    }
    setUpStatus(AudioRecorderStatus::aboutToRecord);
    QFile file(recordedAudioFilePath_);
    file.remove();
    recordedAudioFilePath_ = "";
}

void
AudioRecordView::switchToAboutToRecordPage()
{
    ui->timeDisplay->setVisible(false);
    ui->startAndFinishRecordBtn->setVisible(true);
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
    ui->playBtn->setVisible(false);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(false);
}

void
AudioRecordView::switchToRecordedPage()
{
    ui->timeDisplay->setVisible(false);
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    ui->reRecordBtn->setVisible(true);
    ui->sendBtn->setVisible(true);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(false);
}

void
AudioRecordView::switchToPlayingPage()
{
    ui->timeDisplay->setVisible(true);
    ui->startAndFinishRecordBtn->setVisible(false);
    ui->playBtn->setVisible(true);
    ui->reRecordBtn->setVisible(false);
    ui->sendBtn->setVisible(false);
    ui->stopAndDeleteBtn->setVisible(true);
    ui->stopPlayingBtn->setVisible(true);
}

void
AudioRecordView::startAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recording);
    recordedAudioFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(true));
}

void
AudioRecordView::finishAudioRecord()
{
    setUpStatus(AudioRecorderStatus::recorded);
    LRCInstance::avModel().stopLocalRecorder(recordedAudioFilePath_.toStdString());
}

void
AudioRecordView::interruptAudioRecord(bool willDeleteFile)
{
    if(willDeleteFile) {
        deleteAudioRecord();
    } else {
        if(status_ == AudioRecorderStatus::recording) {
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
    // TODO: start to play and set the finish event to "recorded page"
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
}

void
AudioRecordView::on_playBtn_toggled(bool checked)
{
    setUpStatus(AudioRecorderStatus::playing);
    if(checked) {
        // TODO: play current audio file
    } else {
        // TODO: pause the play
    }

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
        if(checked)
        {
            ui->startAndFinishRecordBtn->toggled(!checked);
            return;
        }
        startAudioRecord();
        break;
    case AudioRecorderStatus::recording:
        if(!checked)
        {
            ui->startAndFinishRecordBtn->toggled(!checked);
            return;
        }
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

void AudioRecordView::on_stopPlayingBtn_toggled(bool checked)
{
    if(status_ != AudioRecorderStatus::playing)
    {
        return;
    }
    if(checked)
    {
        // pause audio play
    }
    else
    {
        // restart the audio play
    }
}

void AudioRecordView::on_audioPlayFinish()
{
    setUpStatus(AudioRecorderStatus::recorded);
    //
}
