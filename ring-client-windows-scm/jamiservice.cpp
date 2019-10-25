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

#include "jamiservice.h"

JamiProcessMonitor::JamiProcessMonitor(PWSTR serviceName,
                                       BOOL canStop,
                                       BOOL canShutdown,
                                       BOOL canPauseContinue) :
    ServiceBase(serviceName, canStop, canShutdown, canPauseContinue),
    stopping_(FALSE),
    killProcessEvent_(CreateEvent(NULL, TRUE, FALSE, NULL)),
    serviceThread_(NULL)
{
    // Create a manual-reset event that is not signaled at first to indicate
    // the stopped signal of the service.
    // (Use to indicate that service working is done in the thread)
    // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventa

    if (killProcessEvent_ == NULL) {
        throw GetLastErrorAsPWSTR();
    }
}

JamiProcessMonitor::~JamiProcessMonitor()
{
    if (killProcessEvent_) {
        CloseHandle(killProcessEvent_);
        killProcessEvent_ = NULL;
    }
}

void
JamiProcessMonitor::onStartCallback(DWORD argc, PWSTR * argv)
{
    DWORD id;
    writeEventLogEntry(getServiceName(), L"JamiService is started", EVENTLOG_INFORMATION_TYPE);

    serviceThread_ = CreateThread(0, 0,
                                 (LPTHREAD_START_ROUTINE)serviceWorkerThread,
                                 (void*)this, 0, &id);
    if (serviceThread_ == NULL) {
        throw GetLastErrorAsPWSTR();
    } else {
        stopping_ = FALSE;
    }
}

void
JamiProcessMonitor::onStopCallback()
{
    writeEventLogEntry(getServiceName(), L"JamiService is stopped", EVENTLOG_INFORMATION_TYPE);

    // Indicate that the service is stopping and wait for the finish of the
    // main service function (ServiceWorkerThread).

    stopping_ = TRUE;
    if (WaitForSingleObject(killProcessEvent_, INFINITE) != WAIT_OBJECT_0) {
        throw GetLastErrorAsPWSTR();
    }
}

DWORD
JamiProcessMonitor::serviceWorkerThread(void* param)
{
    JamiProcessMonitor* currentInstance = (JamiProcessMonitor*)param;
    
    HANDLE hProcess;
    std::wstring monitor_process_name { L"Jami.exe" };

    while (!currentInstance->stopping_) {
        writeEventLogEntry(currentInstance->getServiceName(), L"JamiService is is laughing at you!", EVENTLOG_INFORMATION_TYPE);
        if (findProcessHandle(currentInstance->getServiceName(), monitor_process_name, hProcess)) {
            writeEventLogEntry(currentInstance->getServiceName(), L"Found Jami.exe", EVENTLOG_INFORMATION_TYPE);
        }
        Sleep(4000);
    }

    // Signal the stopped event.
    SetEvent(currentInstance->killProcessEvent_);

    return 0;
}
