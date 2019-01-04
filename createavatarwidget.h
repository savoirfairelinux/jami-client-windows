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

#include <QPixmap>
#include <QGraphicsScene>
#include <QtMultimedia\QMediaPlayer>
#include <QtMultimediaWidgets\QCameraViewfinder>
#include <QtMultimedia\QCameraImageCapture>

namespace Ui {
    class CreateAvatarWidget;
}

class CreateAvatarWidget : public QWidget
{
    Q_OBJECT
    CreateAvatarWidget(const CreateAvatarWidget& cpy);


public:
    CreateAvatarWidget(QWidget* parent = nullptr);
    ~CreateAvatarWidget();


private slots:
    void beginDisplayModeSlot();
    void beginCameraModeSlot();
    void beginEditModeSlot();

    void openFileManagerSlot();
    void imageCaptureSlot(int useless, const QString& text);

private:
    Ui::CreateAvatarWidget* ui;

    void showAvatar(bool state);
    bool checkCamAvailable();
    void closeCamera();
    void saveAvatarAndExit();
    void configureAvatarScene(const QPixmap& pixMap);

    int avatarSize_;

    QMediaPlayer* mediaPlayer_;

    QCameraViewfinder* videoWidget_;
    QCameraImageCapture* imageCapture_;
    QCamera* camera_;
    QGraphicsScene graphicsScene_;

    QPixmap rawPixmap_;
    QPixmap finalAvatarPixmap_;
};