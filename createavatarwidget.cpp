/**************************************************************************
* Copyright (C) 2018 by Savoir-faire Linux                           *
* Author: Isa Nanic <isa.nanic@savoirfairelinux.com>                      *
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
* along with this program.  If not, see <https://www.gnu.org/licenses/>.   *
**************************************************************************/
#include <QWidget>
#include <QtMultimedia\QCameraInfo>
#include <QFileDialog>
#include <QTimer>

#include "lrcinstance.h"

#include "createavatarwidget.h"
#include "ui_createavatarwidget.h"

CreateAvatarWidget::CreateAvatarWidget(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::CreateAvatarWidget),
    videoWidget_(new QCameraViewfinder(this))
{
    ui->setupUi(this);

    connect(ui->cameraButton, &QPushButton::clicked, this, &CreateAvatarWidget::beginCameraModeSlot);
    connect(ui->openFileButton, &QPushButton::clicked, this, &CreateAvatarWidget::openFileManagerSlot);

    connect(ui->confirmButton, &QPushButton::clicked, this, &CreateAvatarWidget::beginEditModeSlot);
    connect(ui->saveButton, &QPushButton::clicked, this, &CreateAvatarWidget::saveAvatarAndExit);
    connect(ui->cancelButton, &QPushButton::clicked, this, &CreateAvatarWidget::beginDisplayModeSlot);

    avatarSize_ = ui->graphicsView->height();
    ui->graphicsView->setAvatarSize(avatarSize_/2);

    videoWidget_->setGeometry(ui->graphicsView->x(), ui->graphicsView->y(),
        ui->graphicsView->width(), ui->graphicsView->height());

    ui->avatarLabel->setFixedSize(ui->graphicsView->width(), ui->graphicsView->height());
    beginDisplayModeSlot();
}

CreateAvatarWidget::~CreateAvatarWidget()
{
    delete ui;
}

void
CreateAvatarWidget::beginDisplayModeSlot()
{
    closeCamera();

    showAvatar(true);
    ui->saveButton->hide();

    ui->avatarLabel->setPixmap(LRCInstance::getCurrAccPixmap().
        scaledToHeight(ui->graphicsView->height(), Qt::SmoothTransformation));
}

void
CreateAvatarWidget::beginCameraModeSlot()
{
    imageCapture_ = new QCameraImageCapture(camera_, this);

    connect(imageCapture_, SIGNAL(imageSaved(int, const QString&)),
        this, SLOT(imageCaptureSlot(int, const QString&)));

    showAvatar(false);
    ui->saveButton->hide();

    if (checkCamAvailable()) {
        camera_ = new QCamera(QCamera::FrontFace);

        camera_->setViewfinder(videoWidget_);
        camera_->start();

        ui->graphicsView->hide();
        videoWidget_->show();
    } else {
        openFileManagerSlot(); // no camera detected so open file manager
    }
}

void
CreateAvatarWidget::beginEditModeSlot()
{
    if (imageCapture_ && camera_) {
        camera_->setCaptureMode(QCamera::CaptureStillImage);
        imageCapture_->setCaptureDestination(QCameraImageCapture::CaptureToFile);

        if (imageCapture_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToFile)) {
            camera_->start();
            camera_->searchAndLock();
            imageCapture_->capture();
            camera_->unlock();
            camera_->stop();
        }
    }

    showAvatar(false);
    ui->saveButton->show();
    ui->confirmButton->hide();
    closeCamera();
}

void
CreateAvatarWidget::showAvatar(bool state)
{
    ui->cancelButton->setVisible(!state);
    ui->confirmButton->setVisible(!state);
    ui->graphicsView->setVisible(!state);

    ui->cameraButton->setVisible(state);
    ui->openFileButton->setVisible(state);
    ui->avatarLabel->setVisible(state);
}

bool
CreateAvatarWidget::checkCamAvailable()
{
    if (QCameraInfo::availableCameras().count() > 0) {
        return true;
    }
    else {
        return false;
    }
}

void
CreateAvatarWidget::closeCamera()
{
    if (camera_ && camera_->status() != QCamera::State::UnloadedState) {
        camera_->stop();
        camera_->unlock();
    }

    if (imageCapture_) {
        disconnect(imageCapture_);
    }
}

void
CreateAvatarWidget::openFileManagerSlot()
{
    rawPixmap_.load(QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)")));

    if (rawPixmap_.isNull()) {
        beginDisplayModeSlot();
    }
    else {
        configureAvatarScene(rawPixmap_);
        beginEditModeSlot();
    }
}

// save avatar image and exit CreateAvatarWidget
void
CreateAvatarWidget::saveAvatarAndExit()
{
    QRect avatarRect((ui->graphicsView->width() - avatarSize_) / 2 + ui->graphicsView->x() + 1,
        (ui->graphicsView->height() - avatarSize_) / 2 + ui->graphicsView->y() + 1,
        avatarSize_ - 2, avatarSize_ - 2);

    QRegion avatarRegion(avatarRect.x() - ui->graphicsView->x(), avatarRect.y() - ui->graphicsView->y(),
        avatarSize_, avatarSize_, QRegion::Ellipse);
    ui->graphicsView->setMask(avatarRegion);

    finalAvatarPixmap_ = grab(avatarRect);

    if (!finalAvatarPixmap_.isNull()) {
        QByteArray ba;
        QBuffer bu(&ba);
        finalAvatarPixmap_.save(&bu, "PNG");
        LRCInstance::setCurrAccAvatar(ba.toBase64().toStdString());
    }

    beginDisplayModeSlot();
}

void
CreateAvatarWidget::imageCaptureSlot(int useless, const QString& text)
{
    Q_UNUSED(useless);

    rawPixmap_ = rawPixmap_.fromImage(QImage(text));

    if (!rawPixmap_.isNull()) {
        beginEditModeSlot();
    }
    QFile file(text);
    file.remove(); // erase file
}

void
CreateAvatarWidget::configureAvatarScene(const QPixmap& pixMap)
{
    if (!pixMap.isNull()) {
        const int margin = 8;
        QPen pen;
        pen.setStyle(Qt::NoPen);

        graphicsScene_.clear();

        graphicsScene_.addPixmap(pixMap);
        graphicsScene_.addRect(-(pixMap.width()*margin - avatarSize_) / 2,
            -(pixMap.height() * margin - avatarSize_) / 2, pixMap.width() * margin, pixMap.height() * margin, pen, QBrush());
        ui->graphicsView->setScene(&graphicsScene_);
    }
}
