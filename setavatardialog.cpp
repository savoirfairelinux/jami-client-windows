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
* along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
**************************************************************************/

#include <QCameraInfo>
#include <QFileDialog>
#include <QAbstractScrollArea>
#include <QScreen>
#include <QWindow>
#include <QCameraImageCapture>
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
        takePictureButton_(new QPushButton(this))
{
    ui->setupUi(this);

    ui->fileButton->setCheckable(true);
    ui->pictureButton->setCheckable(true);
    ui->pictureButton->setChecked(true);

    connect(ui->cancelButton, &QPushButton::clicked, [=]() {
        done(0);
        }
    );

    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        saveAvatar();
        }
    );

    connect(ui->fileButton, &QPushButton::clicked, [=]() {
        selectFileBtn();
        }
    );

    connect(ui->pictureButton, &QPushButton::clicked, [=]() {
        selectPictureBtn();
        }
    );

    connect(takePictureButton_, &QPushButton::clicked, [=]() {
        captureImage();
        }
    );

    ui->graphicsView->setAvatarSize(avatarSize_);

    videoWidget_->setGeometry(ui->graphicsView->x(), ui->graphicsView->y(),
        ui->graphicsView->width(), ui->graphicsView->height());

    takePictureButton_->setStyleSheet("background: white; border: 0px;");
    takePictureButton_->setText(tr("Take Picture"));

    show();
    pictureMode();
    startCamera();
}

SetAvatarDialog::~SetAvatarDialog()
{
    disconnect(this);
    delete takePictureButton_;
    delete mediaPlayer_;
    delete videoWidget_;
    delete imageCapture_;
    delete camera_;
    delete ui;
}

void
SetAvatarDialog::startCamera()
{
    camera_ = new QCamera;
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

// no support yet for back-facing camera [todo]; no support for vertical image [todo]
void
SetAvatarDialog::pictureMode()
{
    if (checkCamAvailable()) {
        camera_ = new QCamera(QCamera::FrontFace);
        camera_->setViewfinder(videoWidget_);

        ui->graphicsView->hide();
        videoWidget_->show();

        camera_->start();

        takePictureButton_->setGeometry(ui->saveButton->x(), ui->saveButton->y(),
            ui->saveButton->width(), ui->saveButton->height());
        ui->saveButton->hide();
        takePictureButton_->show();
    }
    else {
        openFileManager(); // no camera detected so open file manager
    }
}

void
SetAvatarDialog::editMode()
{
    camera_->stop();
    camera_->unlock();

    ui->fileButton->setChecked(false);
    ui->pictureButton->setChecked(false);

    takePictureButton_->hide();
    ui->saveButton->show();

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
    camera_->setCaptureMode(QCamera::CaptureStillImage);
    imageCapture_->setCaptureDestination(QCameraImageCapture::CaptureToFile);

    if (imageCapture_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToFile)) {
        camera_->searchAndLock();
        camera_->start();
        imageCapture_->capture("catsarerarebutfurryandsquishyandwhatarethechancesthatsomeonehasafilewiththisname.png");
    }
}

// save avatar image and exit SetAvatarDialog
void
SetAvatarDialog::saveAvatar()
{
    avatarSize_ = avatarSize_ * 2;
    QRect avatarRect((this->width() - avatarSize_)/2, (this->height() - avatarSize_)/2,
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
}

void
SetAvatarDialog::imageCaptureSlot(int useless, const QString& text)
{
    Q_UNUSED(useless);

    rawPixmap_ = rawPixmap_.fromImage(QImage(text));

    if (!rawPixmap_.isNull()){
        editMode();
    }
    QFile file(text); // not ideal to store to disk (use buffer) [todo]
    file.remove();
}

void
SetAvatarDialog::selectPictureBtn()
{
    ui->pictureButton->setChecked(true);
    ui->fileButton->setChecked(false);
    pictureMode();
}

void
SetAvatarDialog::selectFileBtn()
{
    ui->pictureButton->setChecked(false);
    ui->fileButton->setChecked(true);
    openFileManager();
}
