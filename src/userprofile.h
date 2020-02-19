/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include "widgethelpers.h"
#include "lrcinstance.h"
#include "utils.h"

#include <QQuickImageProvider>

class UserProfile : public QmlPopupWidget
{
    Q_OBJECT

public:
    explicit UserProfile(lrc::api::conversation::Info convInfo, QWidget* parent = nullptr);
    ~UserProfile();
};

class UserProfileImageProvider : public QObject, public QQuickImageProvider
{

private:
    lrc::api::conversation::Info convInfo_;

public:
    UserProfileImageProvider(lrc::api::conversation::Info convInfo) :
        QQuickImageProvider(QQuickImageProvider::Image)
    {
        convInfo_ = convInfo;
    }

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        Q_UNUSED(size);
        Q_UNUSED(requestedSize);

        QSize scaleSize;
        QImage image;
        if (id.right(7) == "_qr.png") {
            int width = 200;
            int height = 200;
            scaleSize = QSize(width, height);
            auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo_.participants.at(0));
            image = Utils::setupQRCode(contact.profileInfo.uri, 0);
        } else {
            return Utils::conversationPhoto(convInfo_.uid, LRCInstance::getCurrentAccountInfo());
        }
        auto scaledImage = image.scaled(scaleSize, Qt::KeepAspectRatio);
        return scaledImage;
    }
};