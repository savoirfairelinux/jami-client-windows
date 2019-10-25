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

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>

namespace Ui {
class AudioRecordView;
}

class AudioRecordView : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(bool deleteFileWhenInterrupt READ getDeleteFileWhenInterrupt WRITE setDeleteFileWhenInterrupt DESIGNABLE true NOTIFY deleteFileWhenInterruptChanged);

public:
    enum class AudioRecorderStatus
    {
        aboutToRecord,
        recording,
        recorded,
        playing
    };

    bool getDeleteFileWhenInterrupt();
    void setDeleteFileWhenInterrupt(bool value);

signals:
    void deleteFileWhenInterruptChanged();
    void recordControllerShouldCloseSignal();

public:
    explicit AudioRecordView(QWidget *parent = nullptr);
    ~AudioRecordView();

private:
    void switchToAboutToRecordPage();
    void switchToRecordingPage();
    void switchToRecordedPage();
    void switchToPlayingPage();

    void deleteAudioRecord();

    void startAudioRecord();
    void finishAudioRecord();
    void interruptAudioRecord(bool willDeleteFile);
    void playAudio();


private slots:
    void on_audioPlayFinish(QMediaPlayer::MediaStatus status);
    void on_audioPlayProgressUpdate(qint64 position);
    void on_audioRecordProgressUpdate();

    void on_playBtn_toggled(bool checked);
    void on_startAndFinishRecordBtn_toggled(bool checked);

    void on_sendBtn_pressed();
    void on_reRecordBtn_pressed();
    void on_stopAndDeleteBtn_pressed();
    void on_stopPlayingBtn_pressed();

    void on_audioRecordViewClose();;

private:
    void setUpStatus(AudioRecorderStatus status);
    void initializeOverlayBtnCheckState(QPushButton* btn, bool isToCheck);
    QString convertTimeDisplayFromMilisecond(qint64 milisecond);

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    bool deleteFileWhenInterrupt_ = true;
    QString recordedAudioFilePath_ = "";
    AudioRecorderStatus status_;
    int currentTime_ = 0;
    QTimer* recordTimer_;

    QMediaPlayer* audioPlayer_;
    Ui::AudioRecordView* ui;
};
