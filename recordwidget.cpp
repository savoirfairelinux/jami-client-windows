/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
* Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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
#include "ui_recordwidget.h"

#include "lrcinstance.h"

#include <QtConcurrent/QtConcurrent>

RecordWidget::RecordWidget(QWidget *parent) :
    PopupWidget(parent, Qt::black, PopupDialog::SpikeLabelAlignment::AlignHCenter),
    ui(new Ui::RecordWidget)
{
    ui->setupUi(this);
    recordOverlay_ = new RecordOverlay(this);
    previewWidget_ = new VideoRecordPreviewWidget(this);
    recordOverlay_->setDrawRoundedCorner(true);
}

RecordWidget::~RecordWidget()
{
}

bool
RecordWidget::actionToStartRecord()
{
    bool isSuccessful = false;
    if(!isAudio_ && !LRCInstance::renderer()->isPreviewing()){return isSuccessful;}
    try {
        recordedFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(isAudio_));
        isSuccessful = true;
    }
    catch (...) {
        qDebug() << "The start of record fails";
    }
    return isSuccessful;
}

bool
RecordWidget::actionToFinishRecord()
{
    bool isSuccessful = false;
    try {
        LRCInstance::avModel().stopLocalRecorder(recordedFilePath_.toStdString());
        if (!isAudio_) {
            previewWidget_->toPaintingBackground(false);
            previewWidget_->toDrawLastFrame(true);
            LRCInstance::avModel().stopPreview();
        }
        isSuccessful = true;
    }
    catch (...) {
        qDebug() << "The finish of record fails";
    }
    return isSuccessful;
}

bool
RecordWidget::actionToReRecord()
{
    QtConcurrent::run(
        [this] {
            Utils::oneShotConnect(&LRCInstance::avModel(), &lrc::api::AVModel::recordPlaybackStopped,
                [this] {
                        deleteRecordedFileDetached(recordedFilePath_);
                        recordedFilePath_ = QString::fromStdString(LRCInstance::avModel().startLocalRecorder(isAudio_));
                     });
            LRCInstance::avModel().stopLocalRecorder(recordedFilePath_.toStdString());
        });
    return true;
}

bool
RecordWidget::actionToDeleteRecord()
{
    deleteRecordedFileDetached(recordedFilePath_);
    if (!isAudio_)
        LRCInstance::avModel().startPreview();
    return true;
}

bool
RecordWidget::actionToSend()
{
    bool isSuccessful = false;
    // send out the stored video file
    QFileInfo fi(recordedFilePath_);
    QString fileName = fi.fileName();
    try {
        auto convUid = LRCInstance::getCurrentConvUid();
        LRCInstance::getCurrentConversationModel()->sendFile(convUid, recordedFilePath_.toStdString(), fileName.toStdString());
        // reset file path, make sure that the file that is needed to sent will not be deleted
        recordedFilePath_.clear();
        isSuccessful = true;
    }
    catch (...) {
        qDebug().noquote() << "\n" << "Send file failed !" << "\n";
    }
    return isSuccessful;
}

bool
RecordWidget::isAudio()
{
    return isAudio_;
}

void
RecordWidget::openRecorder(bool isaudio)
{
    isAudio_ = isaudio;
    widgetContainer_->show();
    if(!isAudio_) {
        LRCInstance::avModel().startPreview();
    }
}

void RecordWidget::resizeEvent(QResizeEvent* event)
{
    recordOverlay_->resize(this->size());
    previewWidget_->resize(this->size());

    previewWidget_->stackUnder(recordOverlay_);
}

void
RecordWidget::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    if(!isAudio_) {
        LRCInstance::avModel().stopPreview();
        previewWidget_->toPaintingBackground(true);
        previewWidget_->toDrawLastFrame(false);
    }
}

void
RecordWidget::deleteRecordedFileDetached(const QString deleteFilePath)
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
