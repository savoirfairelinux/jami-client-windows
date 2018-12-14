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

#include "video/previewmanager.h"
#include "profilemodel.h"
#include "profile.h"
#include "account.h"
#include "person.h"

#include "utils.h"


PhotoboothWidget::PhotoboothWidget(QWidget *parent) :
    QWidget(parent),
    fileName_(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first()
              + QStringLiteral("profile.png")),
    ui(new Ui::PhotoboothWidget)
{
    ui->setupUi(this);
    ui->videoFeed->setIsFullPreview(true);
    ui->videoFeed->setPhotoMode(true);
}

PhotoboothWidget::~PhotoboothWidget()
{
    Video::PreviewManager::instance().stopPreview();
    delete ui;
}

void PhotoboothWidget::startBooth()
{
    Video::PreviewManager::instance().stopPreview();
    Video::PreviewManager::instance().startPreview();
    ui->videoFeed->show();
    ui->avatarLabel->hide();
    takePhotoState_ = true;
}

void PhotoboothWidget::stopBooth()
{
    Video::PreviewManager::instance().stopPreview();
    ui->videoFeed->hide();
    ui->avatarLabel->show();
    takePhotoState_ = false;
}

void
PhotoboothWidget::on_importButton_clicked()
{
    fileName_ = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                            "",
                                            tr("Files (*)"));
    if (fileName_.isEmpty()) {
        fileName_ = QStandardPaths::standardLocations(
            QStandardPaths::TempLocation).first()
            + QStringLiteral("profile.png");
    } else {
        Video::PreviewManager::instance().stopPreview();
    }
    auto image = QImage(fileName_);
    auto avatar = image.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
    emit photoTaken(fileName_);
}

void
PhotoboothWidget::on_takePhotoButton_clicked()
{
    if (!takePhotoState_) {
        takePhotoState_ = true;
        ui->takePhotoButton->setIcon(QPixmap(":/images/icons/baseline-camera_alt-24px.svg"));
        emit photoReady();
    } else {
        auto photo = ui->videoFeed->takePhoto();
        Video::PreviewManager::instance().stopPreview();
        photo.save(fileName_);
        auto avatar = photo.scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        ProfileModel::instance().selectedProfile()->person()->setPhoto(avatar);
        ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::getCirclePhoto(avatar, ui->avatarLabel->width())));
        ui->takePhotoButton->setIcon(QPixmap(":/images/icons/baseline-refresh-24px.svg"));
        emit photoTaken(fileName_);
    }
}
