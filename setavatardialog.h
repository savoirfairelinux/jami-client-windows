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

#pragma once

#include <QPushButton>

#include <QAction>
#include <QDialog>
#include <QRegion>
#include <QtMultimedia\QCamera>
#include <QGraphicsScene>
#include <QtMultimedia\QMediaPlayer>
#include <QtMultimedia\QCameraImageCapture>
#include <QtMultimediaWidgets\QCameraViewfinder>
#include <QtMultimedia\QCameraImageCapture>


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
    void pictureMode();
    void captureImage();
    void openFileManager();
    void saveAvatar();

private:
    Ui::SetAvatarDialog* ui;
    void startCamera();

    void configureAvatarScene(const QPixmap& pixMap);
    void editMode();

    bool checkCamAvailable();

    int avatarSize_ = 130;

    QMediaPlayer* mediaPlayer_;

    QCameraViewfinder* videoWidget_;
    QCameraImageCapture* imageCapture_;
    QCamera* camera_;
    QGraphicsScene graphicsScene_;

    QPixmap rawPixmap_;
    QPixmap finalAvatarPixmap_;
};