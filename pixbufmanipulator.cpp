/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
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

#include "pixbufmanipulator.h"

#include <QSize>
#include <QMetaType>
#include <QImage>
#include <QIODevice>
#include <QByteArray>
#include <QBuffer>
#include <QPainter>

#include "person.h"
#include "call.h"
#include "contactmethod.h"
#include "profilemodel.h"
#include "profile.h"
#include "globalinstances.h"

 // new LRC
#include <api/contactmodel.h>
#include <api/conversation.h>
#include <api/account.h>
#include <api/contact.h>

#include "utils.h"
#include "ringthemeutils.h"
#undef interface

QVariant
PixbufManipulator::callPhoto(Call* c, const QSize& size, bool displayPresence)
{
    if (!c || c->type() == Call::Type::CONFERENCE){
        return QVariant::fromValue(Utils::fallbackAvatar(size,
                                                         c->peerContactMethod()->uri().full(),
                                                         c->peerContactMethod()->bestName()));
    }
    return callPhoto(c->peerContactMethod(), size, displayPresence);
}

QVariant
PixbufManipulator::callPhoto(const ContactMethod* cm, const QSize& size, bool displayPresence)
{
    if (cm && cm->contact()) {
        return contactPhoto(cm->contact(), size, displayPresence);
    } else {
        return QVariant::fromValue(Utils::fallbackAvatar(size, cm));
    }
}

QVariant
PixbufManipulator::contactPhoto(Person* p, const QSize& size, bool displayPresence)
{
    Q_UNUSED(displayPresence);
    QImage photo;
    if (p->photo().isValid()) {
        photo = p->photo().value<QImage>();
    } else {
        photo = Utils::fallbackAvatar(IMAGE_SIZE, p->phoneNumbers().at(0));
    }
    return QVariant::fromValue(Utils::scaleAndFrame(photo, size));
}

QVariant PixbufManipulator::personPhoto(const QByteArray& data, const QString& type)
{
    QImage avatar;
    const bool ret = avatar.loadFromData(QByteArray::fromBase64(data), type.toLatin1());
    if (!ret) {
        qDebug() << "vCard image loading failed";
        return QVariant();
    }
    return QPixmap::fromImage(Utils::getCirclePhoto(avatar, avatar.size().width()));
}

QVariant
PixbufManipulator::numberCategoryIcon(const QVariant& p, const QSize& size, bool displayPresence, bool isPresent)
{
    Q_UNUSED(p)
    Q_UNUSED(size)
    Q_UNUSED(displayPresence)
    Q_UNUSED(isPresent)
    return QVariant();
}

QVariant
PixbufManipulator::securityIssueIcon(const QModelIndex& index)
{
    Q_UNUSED(index)
    return QVariant();
}

QByteArray
PixbufManipulator::toByteArray(const QVariant& pxm)
{
    auto image = pxm.value<QImage>();
    QByteArray ba = Utils::QImageToByteArray(image);
    return ba;
}

QVariant
PixbufManipulator::collectionIcon(const CollectionInterface* colItf, PixmapManipulatorI::CollectionIconHint hint) const
{
    Q_UNUSED(colItf)
    Q_UNUSED(hint)
    return QVariant();
}
QVariant
PixbufManipulator::securityLevelIcon(const SecurityEvaluationModel::SecurityLevel level) const
{
    Q_UNUSED(level)
    return QVariant();
}
QVariant
PixbufManipulator::historySortingCategoryIcon(const CategorizedHistoryModel::SortedProxy::Categories cat) const
{
    Q_UNUSED(cat)
    return QVariant();
}
QVariant
PixbufManipulator::contactSortingCategoryIcon(const CategorizedContactModel::SortedProxy::Categories cat) const
{
    Q_UNUSED(cat)
    return QVariant();
}
QVariant
PixbufManipulator::userActionIcon(const UserActionElement& state) const
{
    Q_UNUSED(state)
    return QVariant();
}

QVariant PixbufManipulator::decorationRole(const QModelIndex& index)
{
    Q_UNUSED(index)
    return QVariant();
}

QVariant PixbufManipulator::decorationRole(const Call* c)
{
    QImage photo;
    if (c   && c->peerContactMethod()
            && c->peerContactMethod()->contact()
            && c->peerContactMethod()->contact()->photo().isValid()) {
        photo =  c->peerContactMethod()->contact()->photo().value<QImage>();
    } else {
        Utils::fallbackAvatar(IMAGE_SIZE, c->peerContactMethod());
    }
    return QVariant::fromValue(Utils::scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant PixbufManipulator::decorationRole(const ContactMethod* cm)
{
    QImage photo;
    if (cm  && cm->contact()
            && cm->contact()->photo().isValid()) {
        photo = cm->contact()->photo().value<QImage>();
    } else {
        photo = Utils::fallbackAvatar(IMAGE_SIZE, cm);
    }
    return QVariant::fromValue(Utils::scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant PixbufManipulator::decorationRole(const Person* p)
{
    QImage photo;
    if (p && p->photo().isValid()) {
        photo = p->photo().value<QImage>();
    } else {
        photo = Utils::fallbackAvatar(IMAGE_SIZE, p->phoneNumbers().at(0));
    }
    return QVariant::fromValue(Utils::scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant PixbufManipulator::decorationRole(const Account* acc)
{
    Q_UNUSED(acc)
    return Utils::getCirclePhoto(ProfileModel::instance().
                                 selectedProfile()->person()->photo().value<QImage>(),
                                 IMAGE_SIZE.width());
}

QVariant
PixbufManipulator::decorationRole(const lrc::api::conversation::Info & conversationInfo,
                                  const lrc::api::account::Info & accountInfo)
{
    QImage photo;
    auto contacts = conversationInfo.participants;
    if (!contacts.empty()) {
        try {
            // Get first contact photo
            auto contactUri = contacts.front();
            auto contactInfo = accountInfo.contactModel->getContact(contactUri);
            auto contactPhoto = contactInfo.profileInfo.avatar;
            auto bestName = Utils::bestNameForContact(contactInfo);
            auto bestId = Utils::bestIdForContact(contactInfo);
            if (accountInfo.profileInfo.type == lrc::api::profile::Type::SIP &&
                contactInfo.profileInfo.type == lrc::api::profile::Type::TEMPORARY) {
                photo = Utils::fallbackAvatar(IMAGE_SIZE, QString(), QString());
            }
            else if (accountInfo.profileInfo.type == lrc::api::profile::Type::SIP) {
                photo = Utils::fallbackAvatar(IMAGE_SIZE,
                                              QString::fromStdString("sip:" + bestId),
                                              QString());
            }
            else if (contactInfo.profileInfo.type == lrc::api::profile::Type::TEMPORARY && contactInfo.profileInfo.uri.empty()) {
                photo = Utils::fallbackAvatar(IMAGE_SIZE, QString(), QString());
            }
            else if (!contactPhoto.empty()) {
                QByteArray byteArray(contactPhoto.c_str(), contactPhoto.length());
                photo = personPhoto(byteArray, nullptr).value<QImage>();
            }
            else {
                auto avatarName = contactInfo.profileInfo.uri == bestName ?
                                  QString() :
                                  QString::fromStdString(bestName);
                photo = Utils::fallbackAvatar(IMAGE_SIZE,
                                              QString::fromStdString("ring:" + bestId),
                                              avatarName);
            }
        }
        catch (...) {}
    }
    return QVariant::fromValue(Utils::scaleAndFrame(photo, IMAGE_SIZE));
}