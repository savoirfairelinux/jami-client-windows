/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "video/previewmanager.h"

#include "utils.h"
#include "lrcinstance.h"

PhotoboothWidget::PhotoboothWidget(QWidget *parent) :
    QWidget(parent),
    fileName_(""),
    ui(new Ui::PhotoboothWidget),
    hasAvatar_(false)
{
    ui->setupUi(this);
    ui->videoFeed->setIsFullPreview(true);
    ui->videoFeed->setPhotoMode(true);

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

    takePhotoState_ = true;
    ui->takePhotoButton->setIcon(QPixmap(":/images/icons/baseline-camera_alt-24px.svg"));
}

PhotoboothWidget::~PhotoboothWidget()
{
    Video::PreviewManager::instance().stopPreview();
    delete ui;
}

void PhotoboothWidget::startBooth()
{
    hasAvatar_ = false;
    ui->videoFeed->setResetPreview(true);
    Video::PreviewManager::instance().stopPreview();
    Video::PreviewManager::instance().startPreview();
    ui->videoFeed->show();
    ui->avatarLabel->hide();
    takePhotoState_ = true;
    ui->takePhotoButton->setIcon(QPixmap(":/images/icons/baseline-camera_alt-24px.svg"));
}

void PhotoboothWidget::stopBooth()
{
    Video::PreviewManager::instance().stopPreview();
    ui->videoFeed->hide();
    ui->avatarLabel->show();
    takePhotoState_ = false;
    ui->takePhotoButton->setIcon(QPixmap(":/images/icons/baseline-refresh-24px.svg"));
}

void
PhotoboothWidget::on_importButton_clicked()
{
    Video::PreviewManager::instance().stopPreview();
    auto picturesDir = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    fileName_ = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                             picturesDir,
                                             tr("Image Files (*.jpg, *.jpeg, *.png)"));
    if (fileName_.isEmpty()) {
        Video::PreviewManager::instance().startPreview();
        return;
    }
    Video::PreviewManager::instance().stopPreview();
    auto image = QImage(fileName_);
    auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    avatarPixmap_ = QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width()));
    ui->avatarLabel->setPixmap(avatarPixmap_);
    hasAvatar_ = true;
    stopBooth();
}

void
PhotoboothWidget::on_takePhotoButton_clicked()
{
    if (!takePhotoState_) {
        startBooth();
        return;
    } else {
        auto videoRect = ui->videoFeed->rect();
        QPoint avatarLabelPos = ui->videoFeed->mapTo(this, videoRect.topLeft());
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
                Video::PreviewManager::instance().stopPreview();
                auto photo = ui->videoFeed->takePhoto();
                auto avatar = photo.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
                avatarPixmap_ = QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width()));
                ui->avatarLabel->setPixmap(avatarPixmap_);
                hasAvatar_ = true;
                stopBooth();
            });
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