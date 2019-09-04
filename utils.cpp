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

#include "utils.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <lmcons.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#endif

//Qt
#include <QBitmap>
#include <QObject>
#include <QErrorMessage>
#include <QPainter>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QScreen>

#include <globalinstances.h>

#include "pixbufmanipulator.h"
#include "globalsystemtray.h"
#include "lrcinstance.h"
#include "downloadmanager.h"
#include "updateconfirmdialog.h"
#include "version.h"

bool
Utils::CreateStartupLink(const std::wstring& wstrAppName)
{
#ifdef Q_OS_WIN
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    std::wstring programPath(szPath);

    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += std::wstring(TEXT("\\") + wstrAppName + TEXT(".lnk"));

    return Utils::CreateLink(programPath.c_str(), linkPath.c_str());
#else
    return true;
#endif
}

bool
Utils::CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink) {
#ifdef Q_OS_WIN
    HRESULT hres;
    IShellLink* psl;

    hres = CoCreateInstance(CLSID_ShellLink, NULL,
                            CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        psl->SetPath(lpszPathObj);
        psl->SetArguments(TEXT("--minimized"));

        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
        if (SUCCEEDED(hres))
        {
            hres = ppf->Save(lpszPathLink, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
#else
    Q_UNUSED(lpszPathObj)
    Q_UNUSED(lpszPathLink)
    return true;
#endif
}

void
Utils::DeleteStartupLink(const std::wstring& wstrAppName) {
#ifdef Q_OS_WIN
    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += std::wstring(TEXT("\\") + wstrAppName + TEXT(".lnk"));

    DeleteFile(linkPath.c_str());
#endif
}

bool
Utils::CheckStartupLink(const std::wstring& wstrAppName) {
#ifdef Q_OS_WIN
    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += std::wstring(TEXT("\\") + wstrAppName + TEXT(".lnk"));
    return PathFileExists(linkPath.c_str());
#else
    return true;
#endif
}

const char*
Utils::WinGetEnv(const char* name)
{
#ifdef Q_OS_WIN
    const DWORD buffSize = 65535;
    static char buffer[buffSize];
    if (GetEnvironmentVariableA(name, buffer, buffSize)) {
        return buffer;
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

void
Utils::removeOldVersions()
{
#ifdef Q_OS_WIN
    // As per: https://git.jami.net/savoirfairelinux/ring-client-windows/issues/429
    // NB: As only the 64-bit version of this application is distributed, we will only
    // remove 1. the configuration reg keys for Ring-x64, 2. the startup links for Ring,
    // 3. the winsparkle reg keys. The NSIS uninstall reg keys for Jami-x64 are removed
    // by the MSI installer.
    // Uninstallation of Ring, either 32 or 64 bit, is left to the user.
    // The current version of Jami will attempt to kill Ring.exe upon start if a startup
    // link is found.
    QString node64 = "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node";
    QString hkcuSoftwareKey = "HKEY_CURRENT_USER\\Software\\";
    QString uninstKey = "\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\";
    QString company = "Savoir-Faire Linux";

    // 1. configuration reg keys for Ring-x64
    QSettings(hkcuSoftwareKey + "jami.net\\Ring", QSettings::NativeFormat).remove("");
    QSettings(hkcuSoftwareKey + "ring.cx", QSettings::NativeFormat).remove("");
    // 2. unset Ring as a startup application
    if (Utils::CheckStartupLink(TEXT("Ring"))) {
        qDebug() << "Found startup link for Ring. Removing it and killing Ring.exe.";
        Utils::DeleteStartupLink(TEXT("Ring"));
        QProcess::execute("taskkill /im Ring.exe /f");
    }
    // 3. remove registry entries for winsparkle(both Jami-x64 and Ring-x64)
    QSettings(hkcuSoftwareKey + company, QSettings::NativeFormat).remove("");
#else
    return;
#endif
}

QString
Utils::GetRingtonePath() {
#ifdef Q_OS_WIN
    TCHAR workingDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, workingDirectory);

    QString ringtonePath = QString::fromWCharArray(workingDirectory);
    ringtonePath += QStringLiteral("\\ringtones\\default.opus");
    return ringtonePath;
#else
    return QString("/usr/local");
#endif
}

QString
Utils::GenGUID() {
#ifdef Q_OS_WIN
    GUID gidReference;
    wchar_t *str;
    HRESULT hCreateGuid = CoCreateGuid(&gidReference);
    if (hCreateGuid == S_OK) {
        StringFromCLSID(gidReference, &str);
        auto gStr = QString::fromWCharArray(str);
        return gStr.remove("{").remove("}").toLower();
    }
    else
        return QString();
#else
    return QString("");
#endif
}

QString
Utils::GetISODate() {
#ifdef Q_OS_WIN
    SYSTEMTIME lt;
    GetSystemTime(&lt);
    return QString("%1-%2-%3T%4:%5:%6Z").arg(lt.wYear).arg(lt.wMonth,2,10,QChar('0')).arg(lt.wDay,2,10,QChar('0'))
            .arg(lt.wHour,2,10,QChar('0')).arg(lt.wMinute,2,10,QChar('0')).arg(lt.wSecond,2,10,QChar('0'));
#else
    return QString();
#endif
}

void
Utils::InvokeMailto(const QString& subject,
                    const QString& body,
                    const QString& attachement) {
#ifdef Q_OS_WIN
    HKEY hKey;
    LONG lRes = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"mailto", 0, KEY_READ, &hKey);
    if (lRes != ERROR_FILE_NOT_FOUND) {
        auto addr = QString("mailto:?subject=%1&body=%2").arg(subject).arg(body);
        if (not attachement.isEmpty())
            addr += QString("&attachement=%1").arg(attachement);
        ShellExecute(nullptr, L"open", addr.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
    } else {
        QErrorMessage errorMessage;
        errorMessage.showMessage(QObject::tr("No default mail client found"));
    }
#endif
}

QImage
Utils::getCirclePhoto(const QImage original, int sizePhoto)
{
    QImage target(sizePhoto, sizePhoto, QImage::Format_ARGB32_Premultiplied);
    target.fill(Qt::transparent);

    QPainter painter(&target);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setBrush(QBrush(Qt::white));
    auto scaledPhoto = original
            .scaled(sizePhoto, sizePhoto, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
            .convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int margin = 0;
    if (scaledPhoto.width() > sizePhoto) {
        margin = (scaledPhoto.width() - sizePhoto) / 2;
    }
    painter.drawEllipse(0, 0, sizePhoto, sizePhoto);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.drawImage(0, 0, scaledPhoto, margin, 0);
    return target;
}

QImage
Utils::getRoundedEdgePhoto(const QImage original, int widthPhoto, int heightPhoto, int roundness)
{
    // First, create a transparent image with the same size
    QImage target(widthPhoto, heightPhoto, QImage::Format_ARGB32_Premultiplied);
    target.fill(Qt::transparent);

    //Second, create a painter onto that image
    QPainter painter(&target);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setBrush(QBrush(original));
    // set brush to the image that we want to draw
    // Note that, the Qbrush is in default texture mode

    //Third, scale the original image into the size of the image we created
    auto scaledPhoto = original
            .scaled(widthPhoto, heightPhoto, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation)
            .convertToFormat(QImage::Format_ARGB32_Premultiplied);
    painter.drawRoundedRect(0, 0, widthPhoto, heightPhoto, roundness, roundness);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    // Source-In mode and Draw the image onto the transperant image
    painter.drawImage(0, 0, scaledPhoto, 0, 0);
    return target;
}

void
Utils::setStackWidget(QStackedWidget* stack, QWidget* widget)
{
    if (stack->indexOf(widget) != -1 && stack->currentWidget() != widget) {
        stack->setCurrentWidget(widget);
    }
}

void Utils::showSystemNotification(QWidget* widget, const QString & message, long delay)
{
    GlobalSystemTray::instance().showMessage(message, "", QIcon(":images/jami.png"));
    QApplication::alert(widget, delay);
}

void Utils::showSystemNotification(QWidget* widget,
    const QString & sender,
    const QString & message,
    long delay)
{
    QIcon();
    GlobalSystemTray::instance()
        .showMessage(sender, message, QIcon(":images/jami.png"));
    QApplication::alert(widget, delay);
}

QSize
Utils::getRealSize(QScreen* screen)
{
#ifdef Q_OS_WIN
    DEVMODE dmThisScreen;
    ZeroMemory(&dmThisScreen, sizeof(dmThisScreen));
    EnumDisplaySettings((const wchar_t *)screen->name().utf16(),
                        ENUM_CURRENT_SETTINGS,
                        (DEVMODE*)&dmThisScreen);
    return QSize(dmThisScreen.dmPelsWidth, dmThisScreen.dmPelsHeight);
#else
    return {};
#endif
}

void
Utils::cleanUpdateFiles()
{
    // Delete all logs and msi in the %TEMP% directory before launching
    QString dir = QString(Utils::WinGetEnv("TEMP"));
    QDir log_dir(dir, { "jami*.log" });
    for (const QString& filename : log_dir.entryList()) {
        log_dir.remove(filename);
    }
    QDir msi_dir(dir, { "jami*.msi" });
    for (const QString& filename : msi_dir.entryList()) {
        msi_dir.remove(filename);
    }
    QDir version_dir(dir, { "version" });
    for (const QString& filename : version_dir.entryList()) {
        version_dir.remove(filename);
    }
}

void
Utils::checkForUpdates(bool withUI, QWidget* parent)
{
    Utils::cleanUpdateFiles();
    QString downloadpath = WinGetEnv("TEMP");
    DownloadManager::instance().downloadFile(
        QUrl::fromEncoded("https://dl.jami.net/windows/version"),
        downloadpath,
        withUI,
        [parent, withUI, downloadpath](int status) {
            if (status != 200) {
                if (withUI)
                    QMessageBox::critical(0,
                        QObject::tr("Update"),
                        QObject::tr("Version cannot be verified"));
                return;
            }

            QFile file(downloadpath + "/" + "version");
            if (!file.open(QIODevice::ReadOnly)) {
                if (withUI)
                    QMessageBox::critical(0,
                        QObject::tr("Update"),
                        QObject::tr("File cannnot be opened"));
                return;
            }

            QTextStream in(&file);
            QString onlineVersion = in.readLine();
            file.close();
            auto currentVersion = QString(VERSION_STRING).toULongLong();
            if (onlineVersion.isEmpty()) {
                qWarning() << "No version file found";
            } else if (onlineVersion.toULongLong() > currentVersion) {
                qDebug() << "New version found";
                Utils::applyUpdates(parent);
            } else {
                qDebug() << "No new version found";
                if (withUI) {
                    QMessageBox::information(0,
                        QObject::tr("Update"),
                        QObject::tr("No new version found"));
                }
            }
        });
}

void
Utils::applyUpdates(QWidget* parent)
{
    if (!parent->findChild<UpdateConfirmDialog*>()) {
        UpdateConfirmDialog updateDialog(parent);
        auto ret = updateDialog.exec();
        if (ret != QDialog::Accepted)
            return;
    } else
        return;

    DownloadManager::instance().downloadFile(
        QUrl::fromEncoded("https://dl.jami.net/windows/jami-x64.msi"),
        WinGetEnv("TEMP"),
        true,
        [parent](int status) {
            if (status != 200) {
                QMessageBox::critical(0,
                    QObject::tr("Update"),
                    QObject::tr("Installer download failed, please contact support"));
                return;
            }
            auto args = QString(" /passive /norestart WIXNONUILAUNCH=1");
            auto dir = Utils::WinGetEnv("TEMP");
            auto cmd = "powershell " + QString(dir) + "\\jami-x64.msi"
                + " /L*V " + QString(dir) + "\\jami_x64_install.log" + args;
            auto retq = QProcess::startDetached(cmd);
            if (retq) {
                QCoreApplication::exit();
            }
        });
}

// new lrc helpers

inline std::string
removeEndlines(const std::string& str)
{
    std::string trimmed(str);
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\n'), trimmed.end());
    trimmed.erase(std::remove(trimmed.begin(), trimmed.end(), '\r'), trimmed.end());
    return trimmed;
}

std::string
Utils::bestIdForConversation(const lrc::api::conversation::Info& conv, const lrc::api::ConversationModel& model)
{
    auto contact = model.owner.contactModel->getContact(conv.participants[0]);
    if (!contact.registeredName.empty()) {
        return removeEndlines(contact.registeredName);
    }
    return removeEndlines(contact.profileInfo.uri);
}

std::string
Utils::bestIdForAccount(const lrc::api::account::Info& account)
{
    if (!account.registeredName.empty()) {
        return removeEndlines(account.registeredName);
    }
    return removeEndlines(account.profileInfo.uri);
}

std::string
Utils::bestNameForAccount(const lrc::api::account::Info& account)
{
    if (account.profileInfo.alias.empty()) {
        return bestIdForAccount(account);
    }
    return account.profileInfo.alias;
}

std::string
Utils::bestIdForContact(const lrc::api::contact::Info& contact)
{
    if (!contact.registeredName.empty()) {
        return removeEndlines(contact.registeredName);
    }
    return removeEndlines(contact.profileInfo.uri);
}

std::string
Utils::bestNameForContact(const lrc::api::contact::Info& contact)
{
    auto alias = removeEndlines(contact.profileInfo.alias);
    if (alias.length() == 0) {
        return bestIdForContact(contact);
    }
    return alias;
}

std::string
Utils::bestNameForConversation(const lrc::api::conversation::Info& conv, const lrc::api::ConversationModel& model)
{
    auto contact = model.owner.contactModel->getContact(conv.participants[0]);
    auto alias = removeEndlines(contact.profileInfo.alias);
    if (alias.length() == 0) {
        return bestIdForConversation(conv, model);
    }
    return alias;
}

// returns empty string if only infoHash is available, second best identifier otherwise
std::string
Utils::secondBestNameForAccount(const lrc::api::account::Info& account)
{
    auto alias = removeEndlines(account.profileInfo.alias);
    auto registeredName = removeEndlines(account.registeredName);
    auto infoHash = account.profileInfo.uri;

    if (!alias.length() == 0) { // if alias exists
        if (!registeredName.length() == 0) { // if registeredName exists
            return registeredName;
        }
        else {
            return infoHash;
        }
    }
    else {
        if (!registeredName.length() == 0) { // if registeredName exists
            return infoHash;
        }
        else {
            return "";
        }
    }
}

lrc::api::profile::Type
Utils::profileType(const lrc::api::conversation::Info& conv, const lrc::api::ConversationModel& model)
{
    try {
        auto contact = model.owner.contactModel->getContact(conv.participants[0]);
        return contact.profileInfo.type;
    }
    catch (...) {
        return lrc::api::profile::Type::INVALID;
    }
}

std::string
Utils::formatTimeString(const std::time_t& timestamp)
{
    std::time_t now = std::time(nullptr);
    char interactionDay[64];
    char nowDay[64];
    std::strftime(interactionDay, sizeof(interactionDay), "%D", std::localtime(&timestamp));
    std::strftime(nowDay, sizeof(nowDay), "%D", std::localtime(&now));
    if (std::string(interactionDay) == std::string(nowDay)) {
        char interactionTime[64];
        std::strftime(interactionTime, sizeof(interactionTime), "%R", std::localtime(&timestamp));
        return interactionTime;
    }
    else {
        return interactionDay;
    }
}

lrc::api::ConversationModel::ConversationQueue::const_iterator
Utils::getConversationFromUid(const std::string& uid, const lrc::api::ConversationModel& model) {
    return std::find_if(model.allFilteredConversations().begin(), model.allFilteredConversations().end(),
        [&](const lrc::api::conversation::Info& conv) {
            return uid == conv.uid;
        });
}

lrc::api::ConversationModel::ConversationQueue::const_iterator
Utils::getConversationFromUri(const std::string& uri, const lrc::api::ConversationModel& model) {
    return std::find_if(model.allFilteredConversations().begin(), model.allFilteredConversations().end(),
        [&](const lrc::api::conversation::Info& conv) {
            return uri == conv.participants[0];
        });
}

bool
Utils::isInteractionGenerated(const lrc::api::interaction::Type& type)
{
    return  type == lrc::api::interaction::Type::CALL ||
            type == lrc::api::interaction::Type::CONTACT;
}

bool
Utils::isContactValid(const std::string& contactUid, const lrc::api::ConversationModel& model)
{
    auto contact = model.owner.contactModel->getContact(contactUid);
    return  (contact.profileInfo.type == lrc::api::profile::Type::PENDING ||
            contact.profileInfo.type == lrc::api::profile::Type::TEMPORARY ||
            contact.profileInfo.type == lrc::api::profile::Type::RING ||
            contact.profileInfo.type == lrc::api::profile::Type::SIP) &&
            !contact.profileInfo.uri.empty();
}

QImage
Utils::conversationPhoto(const std::string & convUid, const lrc::api::account::Info& accountInfo)
{
    auto convInfo = getConversationFromUid(convUid, false);
    if (!convInfo.uid.empty()) {
        return GlobalInstances::pixmapManipulator()
            .decorationRole(convInfo, accountInfo)
            .value<QImage>();
    }
    return QImage();
}

QColor
Utils::getAvatarColor(const QString& canonicalUri) {
    if (canonicalUri.isEmpty()) {
        return RingTheme::defaultAvatarColor_;
    }
    auto h = QString(QCryptographicHash::hash(canonicalUri.toLocal8Bit(), QCryptographicHash::Md5).toHex());
    if (h.isEmpty() || h.isNull()) {
        return RingTheme::defaultAvatarColor_;
    }
    auto colorIndex = std::string("0123456789abcdef").find(h.at(0).toLatin1());
    return RingTheme::avatarColors_[colorIndex];
}

// Generate a QImage representing a dummy user avatar, when user doesn't provide it.
// Current rendering is a flat colored circle with a centered letter.
// The color of the letter is computed from the circle color to be visible whaterver be the circle color.
QImage
Utils::fallbackAvatar(const QSize size, const QString& canonicalUriStr, const QString& letterStr)
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
        painter.drawPixmap(overlayRect, QPixmap(":/images/default_avatar_overlay.svg"));
    }

    return avatar;
}

QImage
Utils::fallbackAvatar(const QSize size, const std::string& alias, const std::string& uri)
{
    return fallbackAvatar(size,
        QString::fromStdString(uri),
        QString::fromStdString(alias));
}

QByteArray
Utils::QImageToByteArray(QImage image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    return ba;
}

QImage
Utils::cropImage(const QImage& img)
{
    QRect rect;
    auto w = img.width();
    auto h = img.height();
    if (w > h) {
        return img.copy({ (w - h) / 2, 0, h, h });
    }
    return img.copy({ 0, (h - w) / 2, w, w });
}

QByteArray
Utils::QByteArrayFromFile(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    } else {
        qDebug() << "can't open file";
        return QByteArray();
    }
}

QPixmap
Utils::generateTintedPixmap(const QString& filename, QColor color)
{
    QPixmap px(filename);
    QImage tmpImage = px.toImage();
    for (int y = 0; y < tmpImage.height(); y++) {
        for (int x = 0; x < tmpImage.width(); x++) {
            color.setAlpha(tmpImage.pixelColor(x, y).alpha());
            tmpImage.setPixelColor(x, y, color);
        }
    }
    return QPixmap::fromImage(tmpImage);
}

QPixmap Utils::generateTintedPixmap(const QPixmap& pix, QColor color)
{
    QPixmap px = pix;
    QImage tmpImage = px.toImage();
    for (int y = 0; y < tmpImage.height(); y++) {
        for (int x = 0; x < tmpImage.width(); x++) {
            color.setAlpha(tmpImage.pixelColor(x, y).alpha());
            tmpImage.setPixelColor(x, y, color);
        }
    }
    return QPixmap::fromImage(tmpImage);
}

QImage
Utils::scaleAndFrame(const QImage photo, const QSize& size)
{
    return photo.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QImage
Utils::accountPhoto(const lrc::api::account::Info& accountInfo, const QSize& size)
{
    QImage photo;
    if (!accountInfo.profileInfo.avatar.empty()) {
        QByteArray ba = QByteArray::fromStdString(accountInfo.profileInfo.avatar);
        photo = GlobalInstances::pixmapManipulator().personPhoto(ba, nullptr).value<QImage>();
    } else {
        auto bestId = bestIdForAccount(accountInfo);
        auto bestName = bestNameForAccount(accountInfo);
        QString letterStr = bestId == bestName ? QString() : QString::fromStdString(bestName);
        photo = fallbackAvatar(size,
            QString::fromStdString("ring:" + bestId),
            letterStr);
    }
    return scaleAndFrame(photo, size);
}

lrc::api::conversation::Info
Utils::getConversationFromCallId(const std::string & callId)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    using namespace lrc::api::profile;
    for (int i = toUnderlyingValue(Type::RING); i <= toUnderlyingValue(Type::TEMPORARY); ++i) {
        auto filter = toEnum<lrc::api::profile::Type>(i);
        auto conversations = convModel->getFilteredConversations(filter);
        auto conv = std::find_if(conversations.begin(), conversations.end(),
            [&](const lrc::api::conversation::Info& conv) {
                return callId == conv.callId;
            });
        if (conv != conversations.end()) {
            return *conv;
        }
    }
    return lrc::api::conversation::Info();
}

lrc::api::conversation::Info
Utils::getSelectedConversation()
{
    return getConversationFromUid(LRCInstance::getSelectedConvUid(), false);
}

lrc::api::conversation::Info
Utils::getConversationFromUid(const std::string & convUid, bool filtered)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    if (filtered) {
        auto conversation = getConversationFromUid(convUid, *convModel);
    } else {
        using namespace lrc::api::profile;
        for (int i = toUnderlyingValue(Type::RING); i <= toUnderlyingValue(Type::TEMPORARY); ++i) {
            auto filter = toEnum<lrc::api::profile::Type>(i);
            auto conversations = convModel->getFilteredConversations(filter);
            auto conv = std::find_if(conversations.begin(), conversations.end(),
                [&](const lrc::api::conversation::Info& conv) {
                    return convUid == conv.uid;
                });
            if (conv != conversations.end()) {
                return *conv;
            }
        }
    }
    return lrc::api::conversation::Info();
}

const lrc::api::conversation::Info&
Utils::getCurrentConvInfo()
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    return *Utils::getConversationFromUid(LRCInstance::getSelectedConvUid(), *convModel);
}

void
Utils::swapQListWidgetItems(QListWidget* list, bool down)
{
    QListWidgetItem *current = list->currentItem();
    int currIndex = list->row(current);
    QListWidgetItem *other = list->item(list->row(current) + (down ? 1 : -1));
    int otherIndex = list->row(other);
    QListWidgetItem *temp = list->takeItem(otherIndex);
    down ? list->insertItem(currIndex, temp) : list->insertItem(otherIndex, current);
    down ? list->insertItem(otherIndex, current) : list->insertItem(currIndex, temp);
}

QString
Utils::humanFileSize(qint64 fileSize)
{
    float fileSizeF = static_cast<float>(fileSize);
    float thresh = 1024;

    if(abs(fileSizeF) < thresh) {
        return QString::number(fileSizeF) + " B";
    }
    QString units[] = { "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB" };
    int unit_position = -1;
    do {
        fileSizeF /= thresh;
        ++unit_position;
    } while (abs(fileSizeF) >= thresh && unit_position < units->size() - 1);
    //Round up to two decimal
    fileSizeF = roundf(fileSizeF * 100) / 100;
    return QString::number(fileSizeF) + " " + units[unit_position];
}
