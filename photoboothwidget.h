/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include "utils.h"
#include "previewrender.h"

#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>

namespace Ui {
class PhotoboothWidget;
}

class PhotoboothWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhotoboothWidget(QWidget *parent = 0);
    ~PhotoboothWidget();

    void startBooth(bool isDeviceChanged = false);
    void stopBooth();
    void setAvatarPixmap(const QPixmap& avatarPixmap, bool default = false, bool stopPhotoboothPreview = false);
    const QPixmap& getAvatarPixmap();
    bool hasAvatar();
    void connectRendering();
    void disconnectRendering();

    // hasConnection_ decides whether the video rendering connection is still in photobooth
    // (connection transmitted from other video widget)
    bool isPhotoBoothOpened() { return takePhotoState_;  }
    bool isPhotoBoothConnected() { return hasConnection_;  }
    void resetTakePhotoState(bool state) { takePhotoState_ = state;  }
    // settingsPreviewed_ changed once the setting's preview is previewed
    void setIsSettingsPreviewed(bool state) { settingsPreviewed_ = state; }

signals:
    void enterSettingsWidgetPhotoBoothFromCallWidget(Utils::VideoWidgetSwapType type);
    void enterCallWidgetFromSettingsWidgetPhotoBooth(Utils::VideoWidgetSwapType type);
    void leaveSettingsWidgetPreviewToSettingsWidgetPhotoBooth(Utils::VideoWidgetSwapType type);

private slots:
    void on_importButton_clicked();
    void on_takePhotoButton_clicked();

private:
    void resetToAvatarLabel();

    QString fileName_;
    Ui::PhotoboothWidget *ui;

    PreviewRenderWidget* previewRenderer_;
    QLabel* flashOverlay_;
    QPropertyAnimation *flashAnimation_;
    QPixmap avatarPixmap_;
    bool hasAvatar_;

    bool takePhotoState_ { false };
    bool hasConnection_ { false };
    bool settingsPreviewed_ { false };

signals:
    void photoTaken();
    void clearedPhoto();
};
