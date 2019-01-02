/**************************************************************************
* Copyright (C) 2019-2019 by Savoir-faire Linux                           *
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

#include <QtMultimedia\QCameraInfo>
#include <QFileDialog>
#include <QAbstractScrollArea>
#include <QScreen>
#include <QWindow>
#include <QtMultimedia\QCameraImageCapture>
#include <QBuffer>

#include "setavatardialog.h"
#include "ui_setavatardialog.h"

#include "lrcinstance.h"
#include "accountlistmodel.h"
#include "currentaccountcombobox.h"

SetAvatarDialog::SetAvatarDialog(QWidget* parent)
    :   QDialog(parent),
        ui(new Ui::SetAvatarDialog),
        videoWidget_(new QCameraViewfinder(this)),
        camera_(new QCamera)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, [&]() {
        done(0);
        }
    );

    connect(ui->saveButton, &QPushButton::clicked, this, &SetAvatarDialog::saveAvatar);

    connect(ui->selectFileButton, &QPushButton::clicked, this, &SetAvatarDialog::openFileManager);

    connect(ui->takePictureButton, &QPushButton::clicked, this, &SetAvatarDialog::captureImage);

    connect(ui->cameraButton, &QPushButton::clicked, this, &SetAvatarDialog::pictureMode);

    ui->graphicsView->setAvatarSize(avatarSize_);

    videoWidget_->setGeometry(ui->graphicsView->x(), ui->graphicsView->y(),
        ui->graphicsView->width(), ui->graphicsView->height());

    pictureMode();
    startCamera();

    ui->horizontalLayout_2->update();
}

SetAvatarDialog::~SetAvatarDialog()
{
    disconnect(this);
    delete mediaPlayer_;
    delete videoWidget_;
    delete imageCapture_;
    delete camera_;
    delete ui;
}

void
SetAvatarDialog::startCamera()
{
    imageCapture_ = new QCameraImageCapture(camera_, this);
    camera_->setCaptureMode(QCamera::CaptureViewfinder);
    connect(imageCapture_, SIGNAL(imageSaved(int, const QString&)),
        this, SLOT(imageCaptureSlot(int, const QString&)));
}

bool
SetAvatarDialog::checkCamAvailable()
{
    if (QCameraInfo::availableCameras().count() > 0) {
        return true;
    }
    else {
        return false;
    }
}

// no support yet for back-facing camera [todo]; no support for portrait mode [todo]
void
SetAvatarDialog::pictureMode()
{
    if (checkCamAvailable()) {
        camera_ = new QCamera(QCamera::FrontFace);
        camera_->setViewfinder(videoWidget_);

        ui->graphicsView->hide();
        videoWidget_->show();

        camera_->start();

        ui->saveButton->hide();
        ui->cameraButton->hide();

        ui->takePictureButton->show();
        ui->selectFileButton->show();
    }
    else {
        openFileManager(); // no camera detected so open file manager
    }
}

void
SetAvatarDialog::editMode()
{
    camera_->unlock();
    camera_->stop();

    ui->takePictureButton->hide();

    ui->saveButton->show();
    ui->cameraButton->show();
    ui->selectFileButton->show();

    videoWidget_->hide();
    ui->graphicsView->show();
    configureAvatarScene(rawPixmap_);
}

void
SetAvatarDialog::openFileManager()
{
    rawPixmap_.load(QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)")));
    editMode();
}

void
SetAvatarDialog::configureAvatarScene(const QPixmap& pixMap)
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

void
SetAvatarDialog::captureImage()
{
    startCamera();

    camera_->setCaptureMode(QCamera::CaptureStillImage);
    imageCapture_->setCaptureDestination(QCameraImageCapture::CaptureToFile);

    if (imageCapture_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToFile)) {
        camera_->searchAndLock();
        camera_->start();
        imageCapture_->capture();
    }
}

// save avatar image and exit SetAvatarDialog
void
SetAvatarDialog::saveAvatar()
{
    avatarSize_ *= 2;

    QRect avatarRect((ui->graphicsView->width() - avatarSize_)/2 + ui->graphicsView->x(), (ui->graphicsView->height() - avatarSize_)/2 + ui->graphicsView->y(),
        avatarSize_, avatarSize_);

    QRegion avatarRegion(avatarRect.x() - ui->graphicsView->x(), avatarRect.y() - ui->graphicsView->y(),
        avatarSize_, avatarSize_, QRegion::Ellipse);
    ui->graphicsView->setMask(avatarRegion);

    finalAvatarPixmap_ = grab(avatarRect);

    done(0);
}

void
SetAvatarDialog::done(int exCode)
{
    QDialog::done(exCode);
    if (!finalAvatarPixmap_.isNull()) {
        QByteArray ba;
        QBuffer bu(&ba);
        finalAvatarPixmap_.save(&bu, "PNG");
        emit pixmapSignal(ba.toBase64().toStdString());
    }
    else {
        emit pixmapSignal(std::string());
    }
    camera_->unlock();
    camera_->stop();
}

void
SetAvatarDialog::imageCaptureSlot(int useless, const QString& text)
{
    Q_UNUSED(useless);

    rawPixmap_ = rawPixmap_.fromImage(QImage(text));

    if (!rawPixmap_.isNull()){
        editMode();
    }
    QFile file(text);
    file.remove(); // erase file
}
