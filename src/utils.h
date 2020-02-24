/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
 * Author: Isa Nanic <isa.nanic@savoirfairelinux.com                       *
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

#include "ringthemeutils.h"

#include <string>

#include <QString>
#include <QImage>
#include <QLabel>
#include <QStackedWidget>
#include <QTextDocument>
#include <QItemDelegate>
#include <QMessageBox>
#include <QtGlobal>
#include <QCryptographicHash>
#include <QListWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#include <ciso646>
#undef ERROR
#else
#define LPCWSTR char*
#endif

#include "api/conversationmodel.h"
#include "api/account.h"
#include "api/contactmodel.h"
#include "api/contact.h"

static const QSize IMAGE_SIZE{ 128, 128 };
static float CURRENT_SCALING_RATIO{ 1.0 };

#ifdef BETA
static constexpr bool isBeta = true;
#else
static constexpr bool isBeta = false;
#endif

namespace Utils
{

// system
bool CreateStartupLink(const std::wstring& wstrAppName);
void DeleteStartupLink(const std::wstring& wstrAppName);
bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
bool CheckStartupLink(const std::wstring& wstrAppName);
void removeOldVersions();
const char* WinGetEnv(const char* name);
QString GetRingtonePath();
QString GenGUID();
QString GetISODate();
void InvokeMailto(const QString& subject, const QString& body, const QString& attachement = QString());
void setStackWidget(QStackedWidget *stack, QWidget *widget);
void showSystemNotification(QWidget* widget, const QString& message, long delay = 5000);
void showSystemNotification(QWidget* widget, const QString& sender, const QString& message, long delay = 5000);
QSize getRealSize(QScreen* screen);
void forceDeleteAsync(const QString& path);
QString getChangeLog();
float getCurrentScalingRatio();
void setCurrentScalingRatio(float ratio);

// updates
void cleanUpdateFiles();
void checkForUpdates(bool withUI, QWidget* parent = nullptr);
void applyUpdates(bool updateToBeta, QWidget* parent = nullptr);

// names
std::string bestIdForConversation(const lrc::api::conversation::Info& conv, const lrc::api::ConversationModel& model);
std::string bestIdForAccount(const lrc::api::account::Info & account);
std::string bestNameForAccount(const lrc::api::account::Info & account);
std::string bestIdForContact(const lrc::api::contact::Info & contact);
std::string bestNameForContact(const lrc::api::contact::Info & contact);
std::string bestNameForConversation(const lrc::api::conversation::Info & conv, const lrc::api::ConversationModel & model);
std::string secondBestNameForAccount(const lrc::api::account::Info& account); // returns empty string if only infoHash is available
lrc::api::profile::Type profileType(const lrc::api::conversation::Info & conv, const lrc::api::ConversationModel & model);

// interactions
std::string formatTimeString(const std::time_t& timestamp);
bool isInteractionGenerated(const lrc::api::interaction::Type& interaction);
bool isContactValid(const std::string& contactUid, const lrc::api::ConversationModel& model);
bool getReplyMessageBox(QWidget* widget, const QString& title, const QString& text);

// image
QImage getCirclePhoto(const QImage original, int sizePhoto);
QImage conversationPhoto(const std::string& convUid, const lrc::api::account::Info& accountInfo);
QColor getAvatarColor(const QString& canonicalUri);
QImage fallbackAvatar(const QSize size, const QString& canonicalUriStr, const QString& letterStr = QString());
QImage fallbackAvatar(const QSize size, const std::string& alias, const std::string& uri);
QByteArray QImageToByteArray(QImage image);
QByteArray QByteArrayFromFile(const QString& filename);
QPixmap generateTintedPixmap(const QString& filename, QColor color);
QPixmap generateTintedPixmap(const QPixmap& pix, QColor color);
QImage scaleAndFrame(const QImage photo, const QSize& size = IMAGE_SIZE);
QImage accountPhoto(const lrc::api::account::Info& accountInfo, const QSize& size = IMAGE_SIZE);
QImage cropImage(const QImage& img);
QPixmap pixmapFromSvg(const QString& svg_resource, const QSize& size);
QImage setupQRCode(QString ringID, int margin);

// rounded corner
template<typename T>
void
fillRoundRectPath(QPainter& painter, const T& brushType, const QRect& rectToDraw, qreal cornerRadius, int xTransFormOffset = 0, int yTransFormOffset = 0)
{
    QBrush brush(brushType);
    brush.setTransform(QTransform::fromTranslate(
                       rectToDraw.x() + xTransFormOffset,
                       rectToDraw.y() + yTransFormOffset));
    QPainterPath painterPath;
    painterPath.addRoundRect(rectToDraw, cornerRadius);
    painter.fillPath(painterPath, brush);
}

// time
QString formattedTime(int seconds);

// misc helpers
void swapQListWidgetItems(QListWidget* list, bool down = true);

// Byte to human readable size
QString humanFileSize(qint64 fileSize);

// device plug or unplug enum
enum class DevicePlugStatus
{
    Plugged,
    Unplugged,
    Unchanged
};

template <typename Func1, typename Func2>
void
oneShotConnect(const typename QtPrivate::FunctionPointer<Func1>::Object* sender, Func1 signal, Func2 slot)
{
    QMetaObject::Connection* const connection = new QMetaObject::Connection;
    *connection = QObject::connect(sender, signal, slot);
    QMetaObject::Connection* const disconnectConnection = new QMetaObject::Connection;
    *disconnectConnection = QObject::connect(sender, signal,
        [connection, disconnectConnection] {
            if (connection) {
                QObject::disconnect(*connection);
                delete connection;
            }
            if (disconnectConnection) {
                QObject::disconnect(*disconnectConnection);
                delete disconnectConnection;
            }
        });
}

template <typename Func1, typename Func2>
void
oneShotConnect( const typename QtPrivate::FunctionPointer<Func1>::Object* sender, Func1 signal,
                const typename QtPrivate::FunctionPointer<Func2>::Object* receiver, Func2 slot)
{
    QMetaObject::Connection* const connection = new QMetaObject::Connection;
    *connection = QObject::connect(sender, signal, receiver, slot);
    QMetaObject::Connection* const disconnectConnection = new QMetaObject::Connection;
    *disconnectConnection = QObject::connect(sender, signal,
        [connection, disconnectConnection] {
            if (connection) {
                QObject::disconnect(*connection);
                delete connection;
            }
            if (disconnectConnection) {
                QObject::disconnect(*disconnectConnection);
                delete disconnectConnection;
            }
        });
}

template<class T>
class Blocker {
    T *blocked;
    bool previous;
public:
    Blocker(T *blocked)
        : blocked(blocked),
          previous(blocked->blockSignals(true)) {}
    ~Blocker() { blocked->blockSignals(previous); }
    T *operator->() { return blocked; }
};

template<class T>
inline Blocker<T> whileBlocking(T *blocked) {
    return Blocker<T>(blocked);
}

template<typename T>
void
setElidedText(T* object, const QString &text, Qt::TextElideMode mode = Qt::ElideMiddle, int padding = 32) {
    QFontMetrics metrics(object->font());
    QString clippedText = metrics.elidedText(text, mode, object->width() - padding);
    object->setText(clippedText);
}

template<typename E>
constexpr inline typename std::enable_if<   std::is_enum<E>::value,
    typename std::underlying_type<E>::type
>::type
toUnderlyingValue(E e) noexcept
{
    return static_cast<typename std::underlying_type<E>::type >(e);
}

template<typename E, typename T>
constexpr inline typename std::enable_if<   std::is_enum<E>::value && std::is_integral<T>::value,
    E
>::type
toEnum(T value) noexcept
{
    return static_cast<E>(value);
}

template<typename T>
ptrdiff_t
indexInVector(const std::vector<T>& vec, const T& item)
{
    auto it = std::find(vec.begin(), vec.end(), item);
    if (it == vec.end()) {
        return -1;
    }
    return std::distance(vec.begin(), it);
}
}
