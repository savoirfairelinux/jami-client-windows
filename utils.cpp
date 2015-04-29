/***************************************************************************
 * Copyright (C) 2015 by Savoir-Faire Linux                                *
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

bool
Utils::CreateStartupLink() {
    TCHAR userHome[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userHome);

    TCHAR workingDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, workingDirectory);

    std::wstring programPath(workingDirectory);
    programPath += TEXT("\\RingClientWindows.exe");

    std::wstring linkPath(userHome);
    linkPath += TEXT("\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Ring.lnk");

    return Utils::CreateLink(programPath.c_str(), linkPath.c_str());
}

bool
Utils::CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink) {
    HRESULT hres;
    IShellLink* psl;

    hres = CoCreateInstance(CLSID_ShellLink, NULL,
                            CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;
        psl->SetPath(lpszPathObj);

        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
        if (SUCCEEDED(hres))
        {
            hres = ppf->Save(lpszPathLink, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

void
Utils::DeleteStartupLink() {
    TCHAR userHome[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userHome);

    std::wstring linkPath(userHome);
    linkPath += TEXT("\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Ring.lnk");

    DeleteFile(linkPath.c_str());
}

bool
Utils::CheckStartupLink() {
    TCHAR userHome[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userHome);

    std::wstring linkPath(userHome);
    linkPath += TEXT("\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\Ring.lnk");
    return PathFileExists(linkPath.c_str());
}

