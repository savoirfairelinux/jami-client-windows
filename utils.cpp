/***************************************************************************
 * Copyright (C) 2015-2017 by Savoir-faire Linux                                *
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
    linkPath += TEXT("\\Ring.lnk");

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
    linkPath += TEXT("\\Ring.lnk");

    DeleteFile(linkPath.c_str());
#endif
}

bool
Utils::CheckStartupLink() {
#ifdef Q_OS_WIN
    TCHAR startupPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupPath);

    std::wstring linkPath(startupPath);
    linkPath += TEXT("\\Ring.lnk");
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
    ringtonePath += QStringLiteral("\\ringtones\\default.wav");

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
