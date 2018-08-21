/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                           *
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
#include <QCryptographicHash>

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

static const QSize IMAGE_SIZE {128, 128};

QColor
getAvatarColor(const QString& canonicalUri) {
    if (canonicalUri.isEmpty()) {
        return RingTheme::defaultAvatarColor_;
    }
    auto h = QString(QCryptographicHash::hash(canonicalUri.toLocal8Bit(), QCryptographicHash::Md5).toHex());
    if (h.isEmpty() || h.isNull()) {
        return RingTheme::defaultAvatarColor_;
    }
    uint8_t colorsLength = sizeof(RingTheme::avatarColors_) / sizeof(QColor);
    auto colorIndex = std::string("0123456789abcdef").find(h.at(0).toLatin1());
    return RingTheme::avatarColors_[colorIndex];
}

// Generate a QImage representing a dummy user avatar, when user doesn't provide it.
// Current rendering is a flat colored circle with a centered letter.
// The color of the letter is computed from the circle color to be visible whaterver be the circle color.
static QImage
fallbackAvatar(const QSize size, const QString& canonicalUriStr, const QString& letterStr = QString())
{
    // We start with a transparent avatar
    QImage avatar(size, QImage::Format_ARGB32);
    avatar.fill(Qt::transparent);

    // We pick a color based on the passed character
    QColor avColor = getAvatarColor(canonicalUriStr);

    // We draw a circle with this color
    QPainter painter(&avatar);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::transparent);
    painter.setBrush(avColor.lighter(110));
    painter.drawEllipse(avatar.rect());

    // If a letter was passed, then we paint a letter in the circle,
    // otherwise we draw the default avatar icon
    QString letterStrCleaned(letterStr);
    letterStrCleaned.remove(QRegExp("[\\n\\t\\r]"));
    if (!letterStr.isEmpty()) {
        auto letter = letterStr.at(0).toUpper().toLatin1();
        QFont font("Arial", avatar.height() / 2.66667, QFont::Medium);
        painter.setFont(font);
        painter.setPen(Qt::white);
        painter.drawText(avatar.rect(), QString(letter), QTextOption(Qt::AlignCenter));
    } else {
        QRect overlayRect = avatar.rect();
        qreal margin = (0.05 * overlayRect.width());
        overlayRect.moveLeft(overlayRect.left() + margin * 0.5);
        overlayRect.moveTop(overlayRect.top() + margin * 0.5);
        overlayRect.setWidth(overlayRect.width() - margin);
        overlayRect.setHeight(overlayRect.height() - margin);
        painter.drawPixmap(overlayRect, QPixmap(":/images/default-avatar-overlay.svg"));
    }

    return avatar;
}

QImage
fallbackAvatar(const QSize size, const ContactMethod* cm)
{
    if (cm == nullptr) {
        return QImage();
    }
    QImage image;
    auto letterStr = QString();
    if (cm->uri().userinfo() != cm->bestName()) {
        letterStr = cm->bestName();
    }
    image = fallbackAvatar( size,
                            cm->uri().full(),
                            letterStr);
    return image;
}

QImage
fallbackAvatar(const QSize size, const std::string& alias, const std::string& uri)
{
    return fallbackAvatar(size,
                          QString::fromStdString(uri),
                          QString::fromStdString(alias));
}

/*Namespace Interfaces collide with QBuffer*/
QByteArray QImageToByteArray(QImage image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return ba;
}

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
        return QVariant::fromValue(fallbackAvatar(size, cm));
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
        photo = fallbackAvatar(IMAGE_SIZE, p->phoneNumbers().at(0));
    }
    return QVariant::fromValue(scaleAndFrame(photo, size));
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
    QByteArray ba = QImageToByteArray(image);
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
        fallbackAvatar(IMAGE_SIZE, c->peerContactMethod());
    }
    return QVariant::fromValue(scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant PixbufManipulator::decorationRole(const ContactMethod* cm)
{
    QImage photo;
    if (cm  && cm->contact()
            && cm->contact()->photo().isValid()) {
        photo = cm->contact()->photo().value<QImage>();
    } else {
        photo = fallbackAvatar(IMAGE_SIZE, cm);
    }
    return QVariant::fromValue(scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant PixbufManipulator::decorationRole(const Person* p)
{
    QImage photo;
    if (p && p->photo().isValid()) {
        photo = p->photo().value<QImage>();
    } else {
        photo = fallbackAvatar(IMAGE_SIZE, p->phoneNumbers().at(0));
    }
    return QVariant::fromValue(scaleAndFrame(photo, IMAGE_SIZE));
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
                photo = fallbackAvatar(IMAGE_SIZE, QString(), QString());
            }
            else if (accountInfo.profileInfo.type == lrc::api::profile::Type::SIP) {
                photo = fallbackAvatar(IMAGE_SIZE,
                                       QString::fromStdString("sip:" + bestId),
                                       QString());
            }
            else if (contactInfo.profileInfo.type == lrc::api::profile::Type::TEMPORARY && contactInfo.profileInfo.uri.empty()) {
                photo = fallbackAvatar(IMAGE_SIZE, QString(), QString());
            }
            else if (!contactPhoto.empty()) {
                QByteArray byteArray(contactPhoto.c_str(), contactPhoto.length());
                photo = personPhoto(byteArray, nullptr).value<QImage>();
            }
            else {
                auto avatarName = contactInfo.profileInfo.uri == bestName ?
                                  QString() :
                                  QString::fromStdString(bestName);
                photo = fallbackAvatar(IMAGE_SIZE,
                                       QString::fromStdString("ring:" + bestId),
                                       avatarName);
            }
        }
        catch (...) {}
    }
    return QVariant::fromValue(scaleAndFrame(photo, IMAGE_SIZE));
}

QVariant
PixbufManipulator::accountPhoto(const lrc::api::account::Info& accountInfo)
{
    QImage photo;
    if (!accountInfo.profileInfo.avatar.empty()) {
        QByteArray ba = QByteArray::fromStdString(accountInfo.profileInfo.avatar);
        photo = GlobalInstances::pixmapManipulator().personPhoto(ba, nullptr).value<QImage>();
    }
    else {
        auto bestId = Utils::bestIdForAccount(accountInfo);
        auto bestName = Utils::bestNameForAccount(accountInfo);
        photo = fallbackAvatar( IMAGE_SIZE,
                                QString::fromStdString("ring:" + bestId),
                                QString::fromStdString(bestName));
    }
    return QVariant::fromValue(scaleAndFrame(photo, IMAGE_SIZE));
}
