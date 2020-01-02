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
    userProfileView_ = new QQuickView;

    userProfileView_->engine()->addImageProvider(QLatin1String("userProfileImage"), new UserProfileImageProvider(convInfo));
    userProfileView_->setSource(QUrl(QStringLiteral("qrc:/src/UserProfileCard.qml")));

    // setProperty keeps the binding, QQmlProperty().write() removes the binding
    auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants.at(0));
    userProfileView_->rootObject()->setProperty("userImageSource", "image://userProfileImage/user.png");
    userProfileView_->rootObject()->setProperty("qrImageSource", "image://userProfileImage/qr.png");
    QString name = QString::fromStdString(contact.profileInfo.alias);
    QString registeredName = QString::fromStdString(contact.registeredName);
    QString uri = QString::fromStdString(contact.profileInfo.uri);
    userProfileView_->rootObject()->setProperty("userNameText", name);
    userProfileView_->rootObject()->setProperty("registeredNameText", registeredName.isEmpty() ? name : registeredName);
    userProfileView_->rootObject()->setProperty("uriText", uri);

    viewWidget_ = QWidget::createWindowContainer(userProfileView_, parent);
    viewWidget_->setMouseTracking(true);
    viewWidget_->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    viewWidget_->setAttribute(Qt::WA_AlwaysStackOnTop);
    viewWidget_->setMinimumWidth(userProfileView_->rootObject()->property("minWidth").toInt());
    viewWidget_->setMinimumHeight(userProfileView_->rootObject()->property("minHeight").toInt());
}

UserProfile::~UserProfile()
{
}

void
UserProfile::showProfile()
{
    viewWidget_->show();
}
