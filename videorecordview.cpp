/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "videorecordview.h"
#include "ui_videorecordview.h"

#include "lrcinstance.h"
#include "utils.h"

#include <QFile>
#include <QGraphicsOpacityEffect>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

VideoRecordView::VideoRecordView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoRecordView)
{
    ui->setupUi(this);

    ui->redDotLabel->hide();
    ui->ImageDisplayLabel->hide();
    ui->videoStartOrFinishRecordingBtn->setToolTip("Start Recording");

    // Button Hidden
    ui->videoRerecordBtn->hide();
    ui->videoDeleteBtn->hide();
    ui->videoPlayBtn->hide();
    ui->videoSendBtn->hide();
    ui->videoStopPlayingBtn->hide();

    // opacity animation
    auto effect = new QGraphicsOpacityEffect(this);
    effect->setOpacity(maxOverlayOpacity_);
    ui->redDotLabel->setGraphicsEffect(effect);

    flickerFadeAnimation_ = new QPropertyAnimation(ui->redDotLabel, "opacity");
    flickerFadeAnimation_->setTargetObject(effect);
    flickerFadeAnimation_->setDuration(fadeRedDotTime_);
    flickerFadeAnimation_->setStartValue(effect->opacity());
    flickerFadeAnimation_->setEndValue(0);
    flickerFadeAnimation_->setEasingCurve(QEasingCurve::OutQuad);

    connect(flickerFadeAnimation_, &QPropertyAnimation::finished, this, &VideoRecordView::restartOrFinishAnimation);

    // seamless transparent background
    ui->videoRecordContainerWidget->setAttribute(Qt::WA_NoSystemBackground, true);
    ui->videoRecordContainerWidget->setAttribute(Qt::WA_TranslucentBackground, true);

    // timer
    recordTimer_ = new QTimer(this);
    connect(recordTimer_, &QTimer::timeout, this, &VideoRecordView::updateTimer);

    // vignette background
    QPixmap bkgnd(":/images/vignette_bng");
    bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    setPalette(palette);

    connect(LRCInstance::renderer(), &RenderManager::videoDeviceListChanged, this, &VideoRecordView::videoDeviceListChanged);
}

VideoRecordView::~VideoRecordView()
{
    delete ui;
}

void
VideoRecordView::restartOrFinishAnimation()
{
    // to reverse the animation direction
    if (flickerFadeAnimation_->startValue() == 0) {
        flickerFadeAnimation_->setStartValue(maxOverlayOpacity_);
        flickerFadeAnimation_->setEndValue(0);
    } else {
        flickerFadeAnimation_->setStartValue(0);
        flickerFadeAnimation_->setEndValue(maxOverlayOpacity_);
    }
    // force it to stop
    if (forceToStopAnimation_) {
        forceToStopAnimation_ = false;
        return;
    }
    flickerFadeAnimation_->start(QAbstractAnimation::KeepWhenStopped);
}

void
VideoRecordView::on_videoStartOrFinishRecordingBtn_toggled(bool checked)
{
    if (checked) {
        if (!LRCInstance::renderer()->isPreviewing()) {
            ui->videoStartOrFinishRecordingBtn->setOverlayButtonChecked(false);
            return;
        }
        // once button clicked (about to start stage to recording stage)
        recordedVideoFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(false));

        recordTimer_->start(1000);
        flickerFadeAnimation_->start(QAbstractAnimation::KeepWhenStopped);

        ui->videoRerecordBtn->show();
        ui->redDotLabel->show();
        ui->videoStartOrFinishRecordingBtn->setToolTip("Stop Recording");
    } else {
        // once button clicked (recording stage to recorded stage)
        LRCInstance::avModel().stopPreview();
        LRCInstance::avModel().stopLocalRecorder(recordedVideoFilePath_.toStdString());

        // stop draeing black background, start to draw the last frame
        //to indicate the end of recording
        ui->videoRecordWidget->toPaintingBackground(false);
        ui->videoRecordWidget->toDrawLastFrame(true);

        recordTimer_->stop();
        forceToStopAnimation_ = true;
        restartOrFinishAnimation();

        ui->redDotLabel->hide();
        ui->videoStartOrFinishRecordingBtn->hide();
        ui->videoRerecordBtn->hide();
        ui->videoSendBtn->show();
        ui->videoDeleteBtn->show();
    }
}

void
VideoRecordView::on_videoDeleteBtn_pressed()
{
    deleteRecordedFileDetached(recordedVideoFilePath_);
    resetState();
    LRCInstance::avModel().startPreview();
}

void
VideoRecordView::on_videoSendBtn_pressed()
{
    // send out the stored video file
    QFileInfo fi(recordedVideoFilePath_);
    QString fileName = fi.fileName();
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendFile(convUid, recordedVideoFilePath_.toStdString(), fileName.toStdString());
    }
    catch (...) {
        qDebug().noquote() << "\n" << "Send file failed !" << "\n";
    }
    // reset file path, make sure that the file that is needed to sent will not be deleted
    recordedVideoFilePath_.clear();
    emit recordControllerShouldCloseSignal();
}

void
VideoRecordView::on_videoRerecordBtn_pressed()
{
    // make sure that user will not click too quick
    ui->videoDeleteBtn->setEnabled(false);
    ui->videoRerecordBtn->setEnabled(false);
    ui->videoStartOrFinishRecordingBtn->setEnabled(false);
    ui->videoSendBtn->setEnabled(false);

    resetTimer();
    recordTimer_->stop();
    recordTimer_->start(1000);

    reRecord();
}

void VideoRecordView::reRecord()
{
    QtConcurrent::run(
        [this] {
            QThread::msleep(500);
            Utils::oneShotConnect(&LRCInstance::avModel(), &lrc::api::AVModel::recordPlaybackStopped,
                [this] {
                    deleteRecordedFileDetached(recordedVideoFilePath_);
                    recordedVideoFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(false));

                    ui->videoDeleteBtn->setEnabled(true);
                    ui->videoRerecordBtn->setEnabled(true);
                    ui->videoStartOrFinishRecordingBtn->setEnabled(true);
                    ui->videoSendBtn->setEnabled(true);
                });
            LRCInstance::avModel().stopLocalRecorder(recordedVideoFilePath_.toStdString());
        });
}

void
VideoRecordView::deleteRecordedFileDetached(const QString deleteFilePath)
{
    // keep deleting file until the process holding it let go
    // Or the file itself does not exist anymore
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

void
VideoRecordView::resetState()
{
    recordedVideoFilePath_.clear();

    ui->videoStartOrFinishRecordingBtn->show();
    ui->videoStartOrFinishRecordingBtn->setToolTip("Start Recording");
    ui->redDotLabel->hide();

    // Button Hidden
    ui->videoRerecordBtn->hide();
    ui->videoDeleteBtn->hide();
    ui->videoPlayBtn->hide();
    ui->videoSendBtn->hide();
    ui->videoStopPlayingBtn->hide();

    ui->videoStartOrFinishRecordingBtn->setOverlayButtonChecked(false);

    recordTimer_->stop();
    resetTimer();

    ui->videoRecordWidget->toPaintingBackground(true);
    ui->videoRecordWidget->toDrawLastFrame(false);
}

void
VideoRecordView::resetTimer()
{
    currentTime_ = 0;
    ui->timerLabel->setText(Utils::convertTimeDisplayFromMilisecond(currentTime_));
}

void
VideoRecordView::updateTimer()
{
    // update the time display label the recording time
    currentTime_++;
    ui->timerLabel->setText(Utils::convertTimeDisplayFromMilisecond(currentTime_));
}

void
VideoRecordView::hideEvent(QHideEvent* event)
{
    Q_UNUSED(event);
    deleteRecordedFileDetached(recordedVideoFilePath_);
    LRCInstance::avModel().stopLocalRecorder(recordedVideoFilePath_.toStdString());
    LRCInstance::avModel().stopPreview();
    resetState();
}

void
VideoRecordView::videoDeviceListChanged()
{
    if (isVisible()) {
        auto devices = LRCInstance::avModel().getDevices();
        if (devices.size() == 0) {
            emit recordControllerShouldCloseSignal();
        } else {
            LRCInstance::renderer()->startPreviewing(false);
        }
    }
}
