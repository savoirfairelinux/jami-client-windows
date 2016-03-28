/***************************************************************************
 * Copyright (C) 2016 by Savoir-faire Linux                                *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
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

#include "person.h"
#include "call.h"
#include "contactmethod.h"
#include "profilemodel.h"
#include "profile.h"

#include "utils.h"

#undef interface

/*Namespace Interfaces collide with QBuffer*/
QByteArray QImageToByteArray(QImage image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return ba;
}

namespace Interfaces {

PixbufManipulator::PixbufManipulator()
    : fallbackAvatar_(QImage(":/images/user/btn-default-userpic.svg"))
{}

QImage
PixbufManipulator::scaleAndFrame(const QImage photo, const QSize& size)
{
    return photo.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QVariant
PixbufManipulator::callPhoto(Call* c, const QSize& size, bool displayPresence)
{
    if (!c || c->type() == Call::Type::CONFERENCE)
        return QVariant::fromValue(scaleAndFrame(fallbackAvatar_, size));
    return callPhoto(c->peerContactMethod(), size, displayPresence);
}

QVariant
PixbufManipulator::callPhoto(const ContactMethod* n, const QSize& size, bool displayPresence)
{
    if (n && n->contact()) {
        return contactPhoto(n->contact(), size, displayPresence);
    } else {
        return QVariant::fromValue(scaleAndFrame(fallbackAvatar_, size));
    }
}

QVariant
PixbufManipulator::contactPhoto(Person* c, const QSize& size, bool displayPresence)
{
    Q_UNUSED(displayPresence);

    /**
     * try to get the photo
     * otherwise use the fallback avatar
     */

    QImage photo;

    if (c->photo().isValid())
        photo = c->photo().value<QImage>();
    else
        photo = fallbackAvatar_;

    return QVariant::fromValue(scaleAndFrame(photo, size));
}

QVariant PixbufManipulator::personPhoto(const QByteArray& data, const QString& type)
{
    QImage avatar;
    QByteArray ba = type.toLatin1();
    const char* c_str2 = ba.data();
    if (avatar.loadFromData(data.fromBase64(data), c_str2))
        return Utils::getCirclePhoto(avatar, avatar.size().width());
    return fallbackAvatar_;
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
    QByteArray ba = QImageToByteArray(image);
    return ba;
}

QVariant
PixbufManipulator::collectionIcon(const CollectionInterface* interface, PixmapManipulatorI::CollectionIconHint hint) const
{
    Q_UNUSED(interface)
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
    if (c && c->peerContactMethod()
            && c->peerContactMethod()->contact()
            && c->peerContactMethod()->contact()->photo().isValid()) {
        photo =  c->peerContactMethod()->contact()->photo().value<QImage>();
    }
    else
        photo = fallbackAvatar_;
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const ContactMethod* cm)
{
    QImage photo;
    if (cm && cm->contact() && cm->contact()->photo().isValid())
        photo = cm->contact()->photo().value<QImage>();
    else
        photo = fallbackAvatar_;
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const Person* p)
{
    QImage photo;
    if (p && p->photo().isValid())
        photo = p->photo().value<QImage>();
    else
        photo = fallbackAvatar_;
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const Account* acc)
{
    Q_UNUSED(acc)
    return Utils::getCirclePhoto(ProfileModel::instance().
                                     selectedProfile()->person()->photo().value<QImage>(),
                                     imgSize_.width());
}

} // namespace Interfaces
