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

#include "userprofile.h"

UserProfile::UserProfile(lrc::api::conversation::Info convInfo, QWidget* parent) :
    QmlPopupWidget(QUrl(QStringLiteral("")), parent)
{
    // set image provider first, since when adding source, images are needed to be interpretted
    engine()->addImageProvider(QLatin1String("userProfileImage"), new UserProfileImageProvider(convInfo));
    setSource(QUrl(QStringLiteral("qrc:/src/UserProfileCard.qml")));

    auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants.at(0));
    QString name = contact.profileInfo.alias;
    QString registeredName = contact.registeredName;
    QString uri = contact.profileInfo.uri;

    // force updating source, since image provide will only be requested once with the same source
    QString userImageSource = "image://userProfileImage/user_";
    userImageSource = userImageSource.append(name.isEmpty() ? registeredName : name).append(".png");
    QString userQrImageSource = "image://userProfileImage/";
    userQrImageSource = userQrImageSource.append(name.isEmpty() ? registeredName : name).append("_qr.png");

    // setProperty keeps the binding, QQmlProperty().write() removes the binding
    rootObject()->setProperty("userImageSource", userImageSource);
    rootObject()->setProperty("qrImageSource", userQrImageSource);

    rootObject()->setProperty("userNameText", registeredName.isEmpty() ? name : registeredName);
    rootObject()->setProperty("registeredNameText", registeredName.isEmpty() ? "(None)" : registeredName);
    rootObject()->setProperty("uriText", uri);

    // widget setup
    setMouseTracking(true);
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setClearColor(Qt::transparent);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setMinimumWidth(rootObject()->property("minWidth").toInt());
    setMinimumHeight(rootObject()->property("minHeight").toInt());
}

UserProfile::~UserProfile()
{
    engine()->removeImageProvider(QLatin1String("userProfileImage"));
}
