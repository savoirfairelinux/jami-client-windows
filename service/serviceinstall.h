/***************************************************************************
 * Copyright (C) 2019 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>              *
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

#include "servicebase.h"

#include <stdio.h>
#include <windows.h>

void cleanUp(SC_HANDLE& handle);

/**
 * Install the current application as a service to the local scm database.
 *
 * @param serviceName - service name
 * @param displayName - display name of the service
 * @param startType - the service start option.
 *        This parameter can be one of  the following values:
 *
 *        SERVICE_AUTO_START,
 *        SERVICE_BOOT_START,
 *        SERVICE_DEMAND_START,
 *        SERVICE_DISABLED,
 *        SERVICE_SYSTEM_START.
 *
 * @param dependencies - a pointer to a double null-terminated array of null-
 *                       separated names of services or load ordering groups that the system
 *                       must start before this service
 * @param account - the name of the account under which the service runs.
 * @param password - password to the account
 */
void
installService(PWSTR serviceName,
               PWSTR displayName,
               DWORD startType,
               PWSTR dependencies,
               PWSTR account,
               PWSTR password,
               LPCWSTR applicationPath)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    // Open the local default scm database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
    if (!schSCManager) {
        std::wcout << "OpenSCManager failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        cleanUp(schSCManager);
        return;
    }

    // Install the service into SCM by calling CreateService
    schService = CreateService(
        schSCManager,                // SCManager database
        serviceName,                 // Name of service
        displayName,                 // Name to display
        SERVICE_ALL_ACCESS,          // Desired access
        SERVICE_WIN32_OWN_PROCESS,   // Service type
        startType,                   // Service start type
        SERVICE_ERROR_NORMAL,        // Error control type
        applicationPath,             // Service's binary
        NULL,                        // No load ordering group
        NULL,                        // No tag identifier
        NULL,                        // Dependencies
        NULL,                        // Service running account
        NULL                         // Password of the account
    );

    if (schService == NULL) {
        std::wcout << "CreateService failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        cleanUp(schSCManager);
        cleanUp(schService);
        return;
    }

    wprintf(L"%s is installed.\n", serviceName);
}

void
uninstallService(PWSTR serviceName)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        std::wcout << "OpenSCManager failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        cleanUp(schSCManager);
        return;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL) {
        std::wcout << "OpenService failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        cleanUp(schSCManager);
        cleanUp(schService);
        return;
    }

    // Try to stop the service
    if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus)) {
        wprintf(L"Stopping %s.", serviceName);
        Sleep(1000);

        while (QueryServiceStatus(schService, &ssSvcStatus)) {
            if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING) {
                wprintf(L".");
                Sleep(1000);
            } else break;
        }

        if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED) {
            wprintf(L"\n%s is stopped.\n", serviceName);
        } else {
            wprintf(L"\n%s failed to stop.\n", serviceName);
        }
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService)) {
        std::wcout << "DeleteService failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        cleanUp(schSCManager);
        cleanUp(schService);
        return;
    }

    wprintf(L"%s is removed.\n", serviceName);
}

void
cleanUp(SC_HANDLE& handle) {
    // cleanup for allocated resources.
    if (handle) {
        CloseServiceHandle(handle);
        handle = NULL;
    }
}
