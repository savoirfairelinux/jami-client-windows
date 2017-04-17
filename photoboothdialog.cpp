/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "photoboothdialog.h"
#include "ui_photoboothdialog.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "video/previewmanager.h"

PhotoBoothDialog::PhotoBoothDialog(QWidget* parent) :
    QDialog(parent),
    fileName_(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first()
              + QStringLiteral("profile.png")),
    ui(new Ui::PhotoBoothDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    ui->videoFeed->setIsFullPreview(true);
    ui->videoFeed->setPhotoMode(true);
    Video::PreviewManager::instance().startPreview();
}

PhotoBoothDialog::~PhotoBoothDialog()
{
    delete ui;
}

void
PhotoBoothDialog::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event)
    Video::PreviewManager::instance().stopPreview();
}

void
PhotoBoothDialog::on_importButton_clicked()
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
        accept();
    }
}

void
PhotoBoothDialog::on_takePhotoButton_clicked()
{
    auto photo = ui->videoFeed->takePhoto();
    Video::PreviewManager::instance().stopPreview();
    photo.save(fileName_);
    accept();
}
