/**************************************************************************
* Copyright (C) 2015-2019 by Savoir-faire Linux                           *
* Author: Yang Wang <yang.wang@savoirfairelinux.com>                      *
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
// directly related to this file
#include "callaudioonlyavataroverlay.h"
#include "ui_callaudioonlyavataroverlay.h"

CallAudioOnlyAvatarOverlay::CallAudioOnlyAvatarOverlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CallAudioOnlyAvatarOverlay)
{
    ui->setupUi(this);
}

CallAudioOnlyAvatarOverlay::~CallAudioOnlyAvatarOverlay()
{
    delete ui;
}

void
CallAudioOnlyAvatarOverlay::setAvatarVisible(bool visible)
{
    this->setVisible(visible);
    ui->avatarLabel->setVisible(visible);
    ui->nameLabel->setVisible(visible);
}

void
CallAudioOnlyAvatarOverlay::writeAvatarOverlay(const lrc::api::conversation::Info& convInfo)
{
    auto contact = LRCInstance::getCurrentAccountInfo().contactModel->getContact(convInfo.participants.at(0));
    ui->avatarLabel->setPixmap(QPixmap::fromImage(Utils::conversationPhoto(convInfo.uid, LRCInstance::getCurrentAccountInfo())));

    if (contact.profileInfo.type == lrc::api::profile::Type::SIP) {
        ui->nameLabel->setText(contact.profileInfo.uri);
    } else {
        QString name = contact.profileInfo.alias;
        QString id = contact.registeredName;
        ui->nameLabel->setText(name + "\n" + id);
    }
}

void
CallAudioOnlyAvatarOverlay::respondToPauseLabel(bool isPaused)
{
    setAvatarVisible(!isPaused);
}
