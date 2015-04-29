#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <lmcons.h>
#include <shobjidl.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <string>

class Utils
{
public:
    static bool CreateStartupLink();
    static void DeleteStartupLink();
    static bool CreateLink(LPCWSTR lpszPathObj, LPCWSTR lpszPathLink);
    static bool CheckStartupLink();
};

#endif // UTILS_H
