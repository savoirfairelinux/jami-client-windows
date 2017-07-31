/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Anthony Léonard <anthony.leonard@savoirfairelinux.com>          *
 * Author: Olivier Soldano <olivier.soldano@savoirfairelinux.com>          *
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

#include "utils.h"
#include "ringthemeutils.h"
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

QImage
PixbufManipulator::scaleAndFrame(const QImage photo, const QSize& size)
{
    return photo.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QVariant
PixbufManipulator::callPhoto(Call* c, const QSize& size, bool displayPresence)
{
    if (!c || c->type() == Call::Type::CONFERENCE){
        return QVariant::fromValue(fallbackAvatar(size,
                                                  c->peerContactMethod()->uri().userinfo().at(0).toLatin1(),
                                                  c->peerContactMethod()->bestName().at(0).toUpper().toLatin1()));
    }
    return callPhoto(c->peerContactMethod(), size, displayPresence);
}

QVariant
PixbufManipulator::callPhoto(const ContactMethod* n, const QSize& size, bool displayPresence)
{
    if (n && n->contact()) {
        return contactPhoto(n->contact(), size, displayPresence);
    } else {
        return QVariant::fromValue(fallbackAvatar(size,
                                                  n->uri().userinfo().at(0).toLatin1(),
                                                  n->bestName().at(0).toUpper().toLatin1()));
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

    if (c->photo().isValid()){
        photo = c->photo().value<QImage>();
    } else {
        photo = fallbackAvatar(size,
                               c->phoneNumbers().at(0)->uri().userinfo().at(0).toLatin1(),
                               c->phoneNumbers().at(0)->bestName().at(0).toUpper().toLatin1());
    }
    return QVariant::fromValue(scaleAndFrame(photo, size));
}

QVariant PixbufManipulator::personPhoto(const QByteArray& data, const QString& type)
{
    QImage avatar;
    QByteArray ba = type.toLatin1();
    const char* c_str2 = ba.data();
    if (avatar.loadFromData(data.fromBase64(data), c_str2))
        return Utils::getCirclePhoto(avatar, avatar.size().width());
    return fallbackAvatar(imgSize_, '?', '?');
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
        photo = fallbackAvatar(imgSize_,
                               c->peerContactMethod()->uri().userinfo().at(0).toLatin1(),
                               c->peerContactMethod()->bestName().at(0).toUpper().toLatin1());
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const ContactMethod* cm)
{
    QImage photo;
    if (cm && cm->contact() && cm->contact()->photo().isValid())
        photo = cm->contact()->photo().value<QImage>();
    else
        photo = fallbackAvatar(imgSize_,
                               cm->uri().userinfo().at(0).toLatin1(),
                               cm->bestName().at(0).toUpper().toLatin1());
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const Person* p)
{
    QImage photo;
    if (p && p->photo().isValid())
        photo = p->photo().value<QImage>();
    else
        photo = fallbackAvatar(imgSize_,
                               p->phoneNumbers().at(0)->uri().userinfo().at(0).toLatin1(),
                               p->phoneNumbers().at(0)->bestName().at(0).toUpper().toLatin1());
    return QVariant::fromValue(scaleAndFrame(photo, imgSize_));
}

QVariant PixbufManipulator::decorationRole(const Account* acc)
{
    Q_UNUSED(acc)
    return Utils::getCirclePhoto(ProfileModel::instance().
                                     selectedProfile()->person()->photo().value<QImage>(),
                                     imgSize_.width());
}

QImage PixbufManipulator::fallbackAvatar(const QSize size, const char color, const char letter)
{
    // We start with a transparent avatar
    QImage avatar(size, QImage::Format_ARGB32);
    avatar.fill(Qt::transparent);

    // We pick a color based on the passed character
    QColor avColor = RingTheme::avatarColors_[color % 16];

    // We draw a circle with this color
    QPainter painter(&avatar);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);
    painter.setBrush(avColor);
    painter.drawEllipse(avatar.rect());

    // Then we paint a letter in the circle
    QFont segoeFont("Segoe UI", avatar.height()/2); // We use Segoe UI as recommended by Windows guidelines
    painter.setFont(segoeFont);
    painter.setPen(Qt::white);
    QRect textRect = avatar.rect();
    textRect.moveTop(textRect.top()-(avatar.height()/20)); // Empirical value that seems to correct centering nicely
    painter.drawText(textRect, QString(letter), QTextOption(Qt::AlignCenter));

    return avatar;
}

} // namespace Interfaces
