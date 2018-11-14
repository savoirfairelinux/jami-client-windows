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

#pragma once

#include <QPushButton>

#include <QAction>
#include <QDialog>
#include <QRegion>
#include <QCamera>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QCameraImageCapture>
#include <QCameraViewfinder>
#include <QCameraImageCapture>


#include "accountlistmodel.h"
#include "avatargraphicsview.h"
#include "settingswidget.h"

namespace Ui {
    class SetAvatarDialog;
}

class SetAvatarDialog : public QDialog
{
    Q_OBJECT
    SetAvatarDialog(const SetAvatarDialog& cpy);

public:
    SetAvatarDialog(QWidget* parent = nullptr);
    ~SetAvatarDialog();

signals:
    void pixmapSignal(const std::string& avatarInBase64String);

private slots:
    void done(int exCode);
    void imageCaptureSlot(int useless, const QString& text);

private:
    Ui::SetAvatarDialog* ui;
    void startCamera();

    void configureAvatarScene(const QPixmap& pixMap);

    void pictureMode();
    void editMode();

    void captureImage();

    void openFileManager();
    void saveAvatar();

    bool checkCamAvailable();

    int avatarSize_ = 160;

    QPushButton* takePictureButton_;

    QMediaPlayer* mediaPlayer_;

    QCameraViewfinder* videoWidget_;
    QCameraImageCapture* imageCapture_;
    QCamera* camera_;
    QGraphicsScene graphicsScene_;

    QPixmap rawPixmap_;
    QPixmap finalAvatarPixmap_;

    void selectPictureBtn();
    void selectFileBtn();
};