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

    void startBooth();
    void stopBooth();
    void setAvatarPixmap(const QPixmap& avatarPixmap, bool _default = false);
    const QPixmap& getAvatarPixmap();
    bool hasAvatar();

private slots:
    void on_importButton_clicked();
    void on_takePhotoButton_clicked();

private:
    QString fileName_;
    Ui::PhotoboothWidget *ui;

    QLabel* flashOverlay_;
    QPropertyAnimation *flashAnimation_;
    QPixmap avatarPixmap_;
    bool hasAvatar_;

    bool takePhotoState_;

signals:
    void photoTaken();
    void clearedPhoto();
};
