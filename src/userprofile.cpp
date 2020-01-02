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

UserProfile::UserProfile(lrc::api::conversation::Info convInfo, QWidget* parent)
{
    userProfileWidget_ = new QmlPopupWidget(QUrl(QStringLiteral("")), parent);

    userProfileWidget_->engine()->addImageProvider(QLatin1String("userProfileImage"), new UserProfileImageProvider(convInfo));
    userProfileWidget_->setSource(QUrl(QStringLiteral("qrc:/src/UserProfileCard.qml")));

    // setProperty keeps the binding, QQmlProperty().write() removes the binding
    auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants.at(0));
    userProfileWidget_->rootObject()->setProperty("userImageSource", "image://userProfileImage/user.png");
    userProfileWidget_->rootObject()->setProperty("qrImageSource", "image://userProfileImage/qr.png");
    QString name = QString::fromStdString(contact.profileInfo.alias);
    // maximum length of the alias
    if(name.length() > 16) {
        name = name.left(13) + "...";
    }
    QString registeredName = QString::fromStdString(contact.registeredName);
    QString uri = QString::fromStdString(contact.profileInfo.uri);
    userProfileWidget_->rootObject()->setProperty("userNameText", name);
    userProfileWidget_->rootObject()->setProperty("registeredNameText", registeredName.isEmpty() ? name : registeredName);
    userProfileWidget_->rootObject()->setProperty("uriText", uri);

    // widget setup
    userProfileWidget_->setMouseTracking(true);
    userProfileWidget_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    userProfileWidget_->setClearColor(Qt::transparent);
    userProfileWidget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    userProfileWidget_->setMinimumWidth(userProfileWidget_->rootObject()->property("minWidth").toInt());
    userProfileWidget_->setMinimumHeight(userProfileWidget_->rootObject()->property("minHeight").toInt());
}

UserProfile::~UserProfile()
{
}

void
UserProfile::showProfile()
{
    userProfileWidget_->getContainer()->exec();
}
