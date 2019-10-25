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
#include <QTimer>

namespace Ui {
class AudioRecordView;
}

class QPropertyAnimation;

class AudioRecordView : public QWidget
{
    Q_OBJECT

public:
    enum class AudioRecorderStatus
    {
        aboutToRecord,
        recording,
        recorded,
    };

signals:
    void recordControllerShouldCloseSignal();

public:
    explicit AudioRecordView(QWidget *parent = nullptr);
    ~AudioRecordView();

private:
    void switchToAboutToRecordPage();
    void switchToRecordingPage();
    void switchToRecordedPage();

    void startAudioRecord();
    void finishAudioRecord();
    void deleteAudioRecord();

    void showRedDot();
    void fadeRedDotOut();

private slots:
    // define what to do when the button is pressed
    void on_startAndFinishRecordBtn_toggled(bool checked);
    void on_sendBtn_pressed();
    void on_reRecordBtn_pressed();
    void on_stopAndDeleteBtn_pressed();

    void whenAudioRecordProgressUpdate();

    void whenAudioRecordViewClose();;

    void onRedDotShouldToggle();

private:
    void setUpStatus(AudioRecorderStatus status);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;

private:
    QString recordedAudioFilePath_ = "";
    AudioRecorderStatus status_;
    int currentTime_ = 0;
    QTimer recordTimer_;

    QPropertyAnimation* redDotFadeAnim_;
    QTimer redDotAnimTimer_;
    constexpr static qreal maxOverlayOpacity_ = 0.9999999999980000442;
    constexpr static int fadeRedDotTime_ = 1000; //msec
    constexpr static int startfadeRedDotTime_ = 1000; //msec
    bool shouldFadeRedDot_ = true;
    int redDotShowCount_ = 0;

    Ui::AudioRecordView* ui;
};
