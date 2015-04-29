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

