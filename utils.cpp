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
#include <QObject>
#include <QErrorMessage>
#include <QPainter>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QApplication>
#include <QFile>

#include "globalinstances.h"
#include "pixbufmanipulator.h"

#include "globalsystemtray.h"
#include "lrcinstance.h"

bool
Utils::CreateStartupLink()
{
#ifdef Q_OS_WIN
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    std::wstring programPath(szPath);

    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += TEXT("\\Jami.lnk");

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
Utils::DeleteStartupLink() {
#ifdef Q_OS_WIN
    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += TEXT("\\Jami.lnk");

    DeleteFile(linkPath.c_str());
#endif
}

bool
Utils::CheckStartupLink() {
#ifdef Q_OS_WIN
    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += TEXT("\\Jami.lnk");
    return PathFileExists(linkPath.c_str());
#else
    return true;
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

QString
Utils::GetCurrentUserName() {
#ifdef Q_OS_WIN
    wchar_t username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    GetUserName(username, &username_len);
    return QString::fromWCharArray(username, username_len-1);
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

void
Utils::setStackWidget(QStackedWidget* stack, QWidget* widget)
{
    if (stack->indexOf(widget) != -1 && stack->currentWidget() != widget) {
        stack->setCurrentWidget(widget);
    }
}

void Utils::showSystemNotification(QWidget* widget, const QString & message, long delay)
{
    GlobalSystemTray::instance().showMessage("Jami", message);
    QApplication::alert(widget, delay);
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
    auto& convModel = accountInfo.conversationModel;
    auto conversation = Utils::getConversationFromUid(convUid, *convModel);
    if (conversation == (*convModel).allFilteredConversations().end()) {
        return QImage();
    }

    QVariant var = GlobalInstances::pixmapManipulator().decorationRole(*conversation, accountInfo);
    return var.value<QImage>();
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

std::string
Utils::getConversationFromCallId(const std::string& callId)
{
    auto convModel = LRCInstance::getCurrentConversationModel();
    auto conversations = convModel->allFilteredConversations();
    std::string convUid;
    for (auto conversation : conversations) {
        if (conversation.callId == callId) {
            return conversation.uid;
        }
    }
    return "";
}
