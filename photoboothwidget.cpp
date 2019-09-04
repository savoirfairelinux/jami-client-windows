/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "photoboothwidget.h"
#include "ui_photoboothwidget.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QGraphicsOpacityEffect>
#include <QtConcurrent/QtConcurrent>

#include "utils.h"
#include "lrcinstance.h"

PhotoboothWidget::PhotoboothWidget(QWidget *parent) :
    QWidget(parent),
    fileName_(""),
    ui(new Ui::PhotoboothWidget),
    hasAvatar_(false)
{
    ui->setupUi(this);

    previewRenderer_ = PreviewRenderWidget::attachPreview();

    flashOverlay_ = new QLabel(this);
    flashOverlay_->setStyleSheet("background-color:#fff");
    flashOverlay_->hide();
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(this);
    flashOverlay_->setGraphicsEffect(effect);
    flashAnimation_ = new QPropertyAnimation(this);
    flashAnimation_->setTargetObject(effect);
    flashAnimation_->setPropertyName("opacity");
    flashAnimation_->setDuration(600);
    flashAnimation_->setStartValue(1);
    flashAnimation_->setEndValue(0);
    flashAnimation_->setEasingCurve(QEasingCurve::OutCubic);

    Utils::drawBlackCircularImageOntoLabel(ui->previewContainer);
    ui->previewContainer->hide();
    ui->takePhotoButton->setIcon(QIcon(":/images/icons/baseline-camera_alt-24px.svg"));
}

PhotoboothWidget::~PhotoboothWidget()
{
    LRCInstance::avModel().stopPreview();
    delete ui;
}

void
PhotoboothWidget::startBooth(bool isDeviceChanged)
{
    hasAvatar_ = false;
    if (!LRCInstance::getActiveCalls().size() || isDeviceChanged || LRCInstance::getIfCurrentSelectedCallIsAudioOnly()) {
        // if no active calls
        previewRenderer_->connectRendering();
        QtConcurrent::run(
            [this] {
                LRCInstance::avModel().stopPreview();
                LRCInstance::avModel().startPreview();
            });
    }
    takePhotoState_ = true;
    ui->previewContainer->show();
    setUpPreviewRenderer();
    ui->avatarLabel->hide();
    ui->takePhotoButton->setIcon(QIcon(":/images/icons/baseline-camera_alt-24px.svg"));
}

void
PhotoboothWidget::stopBooth()
{
    if ((!LRCInstance::getActiveCalls().size() || LRCInstance::getIfCurrentSelectedCallIsAudioOnly()) && takePhotoState_) {
        // if no active calls
        QtConcurrent::run([this] { LRCInstance::avModel().stopPreview(); });
    }
    resetToAvatarLabel();
}

void
PhotoboothWidget::on_importButton_clicked()
{
    if (!LRCInstance::getActiveCalls().size()) {
        LRCInstance::avModel().stopPreview();
    }
    auto picturesDir = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    fileName_ = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                             picturesDir,
                                             tr("Image Files") + " (*.jpg *.jpeg *.png)");
    if (fileName_.isEmpty()) {
        return;
    }
    auto image = Utils::cropImage(QImage(fileName_));
    auto avatar = image.scaled(224, 224, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    avatarPixmap_ = QPixmap::fromImage(avatar);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
    hasAvatar_ = true;
    emit photoTaken();
    if (!LRCInstance::getActiveCalls().size()) {
        stopBooth();
    } else {
        resetToAvatarLabel();
    }
}

void
PhotoboothWidget::on_takePhotoButton_clicked()
{
    if (!takePhotoState_) { // restart
        emit clearedPhoto();
        startBooth();
        return;
    } else {
        auto videoRect = previewRenderer_->rect();
        QPoint avatarLabelPos = previewRenderer_->mapTo(this, videoRect.topLeft());
        flashOverlay_->setGeometry(
            avatarLabelPos.x(),
            avatarLabelPos.y(),
            videoRect.width(),
            videoRect.height()
        );
        flashOverlay_->show();
        flashAnimation_->start(QPropertyAnimation::KeepWhenStopped);

        QtConcurrent::run(
            [this] {
                auto photo = Utils::cropImage(previewRenderer_->takePhoto());
                auto avatar = photo.scaled(224, 224, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                avatarPixmap_ = QPixmap::fromImage(avatar);
                ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
                hasAvatar_ = true;
                emit photoTaken();
                if (!LRCInstance::getActiveCalls().size()) {
                    stopBooth();
                } else {
                    resetToAvatarLabel();
                }
            });
    }
}

void
PhotoboothWidget::setAvatarPixmap(const QPixmap& avatarPixmap, bool default, bool stopPhotoboothPreview)
{
    // function will be called when entering the setting widget
    // or when settings change from one to another if previously the photo booth
    // is opened and not stopped

    // this is to ensure that when we are in a call,
    // entering the setting, changing from settings to settings,
    // video rendering will be used efficiently
    // see resetPhotoBoothStateWhenSettingChanged in settingswidget.cpp

    ui->avatarLabel->setPixmap(avatarPixmap);
    if (!LRCInstance::getActiveCalls().size() && stopPhotoboothPreview) {
        LRCInstance::avModel().stopPreview();
    }
    resetToAvatarLabel();
    if (default) {
        ui->takePhotoButton->setIcon(QIcon(":/images/icons/round-add_a_photo-24px.svg"));
    }
}

const QPixmap&
PhotoboothWidget::getAvatarPixmap()
{
    return avatarPixmap_;
}

bool
PhotoboothWidget::hasAvatar()
{
    return hasAvatar_;
}

void
PhotoboothWidget::resetToAvatarLabel()
{
    ui->previewContainer->hide();
    ui->avatarLabel->show();
    takePhotoState_ = false;
    if (!hasAvatar_) {
        ui->takePhotoButton->setIcon(QIcon(":/images/icons/round-add_a_photo-24px.svg"));
    } else {
        ui->takePhotoButton->setIcon(QIcon(":/images/icons/baseline-refresh-24px.svg"));
    }
}

void
PhotoboothWidget::setUpPreviewRenderer()
{
    previewRenderer_->setParent(ui->previewContainer);
    previewRenderer_->setGeometry(ui->previewContainer->rect());
    previewRenderer_->resetBorder();
    previewRenderer_->setPhotoMode(true);
    previewRenderer_->setNeedToCenter(false);
    previewRenderer_->show();
}
