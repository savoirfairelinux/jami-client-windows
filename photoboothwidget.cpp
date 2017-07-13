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

PhotoboothWidget::PhotoboothWidget(QWidget *parent) :
    QWidget(parent),
    fileName_(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first()
              + QStringLiteral("profile.png")),
    ui(new Ui::PhotoboothWidget)
{
    ui->setupUi(this);
    ui->videoFeed->setIsFullPreview(true);
    ui->videoFeed->setPhotoMode(true);
    startBooth();
}

PhotoboothWidget::~PhotoboothWidget()
{
    Video::PreviewManager::instance().stopPreview();
    delete ui;
}

void PhotoboothWidget::startBooth()
{
    // // // //
    // stop (or start before) to give Preview manager some time to start
    // TODO go modify the daemon to ensure starting upon calling videomanager::startCamera
    Video::PreviewManager::instance().stopPreview();
    // // // //

    Video::PreviewManager::instance().startPreview();
    ui->videoFeed->show();
}

void PhotoboothWidget::stopBooth()
{
    Video::PreviewManager::instance().stopPreview();
    hide();
}

void
PhotoboothWidget::on_importButton_clicked()
{
    fileName_ = QFileDialog::getOpenFileName(this, tr("Choose File"),
                                            "",
                                            tr("Files (*)"));
    if (fileName_.isEmpty())
        fileName_ = QStandardPaths::standardLocations(
                    QStandardPaths::TempLocation).first()
                + QStringLiteral("profile.png");
    else {
        Video::PreviewManager::instance().stopPreview();
    }
    emit photoTaken(fileName_);
}

void
PhotoboothWidget::on_takePhotoButton_clicked()
{
    auto photo = ui->videoFeed->takePhoto();
    Video::PreviewManager::instance().stopPreview();
    photo.save(fileName_);
    emit photoTaken(fileName_);
}
