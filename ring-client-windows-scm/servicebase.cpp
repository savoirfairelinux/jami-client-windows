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

#include "servicebase.h"

#include <tlhelp32.h>
#include <tchar.h>

 // Initialize service instance.
ServiceBase* ServiceBase::currentService_ = NULL;

BOOL
ServiceBase::scmRegister(ServiceBase &service)
{
    // assign created instance to current service
    // other functions are triggered by this instance
    currentService_ = &service;

    if (currentService_ == NULL)
        return FALSE;

    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { service.serviceName_, serviceMain },
        { NULL, NULL }
    };

    // Enable the scm to call service entry point
    return StartServiceCtrlDispatcher(serviceTable);
}

ServiceBase::ServiceBase(PWSTR serviceName,
    BOOL canStop,
    BOOL canShutdown,
    BOOL canPauseContinue) :
    serviceName_((serviceName == NULL) ? L"" : serviceName),
    statusHandle_(NULL)
{
    // The service runs in its own process.
    serviceStatus_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

    // The service is about to start.
    serviceStatus_.dwCurrentState = SERVICE_START_PENDING;

    // The accepted commands of the service.
    DWORD dwControlsAccepted = 0;
    if (canStop)
        dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    if (canShutdown)
        dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    if (canPauseContinue)
        dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    serviceStatus_.dwControlsAccepted = dwControlsAccepted;

    serviceStatus_.dwWin32ExitCode = NO_ERROR;
    serviceStatus_.dwServiceSpecificExitCode = 0;
    serviceStatus_.dwCheckPoint = 0;
    serviceStatus_.dwWaitHint = 0;
}

ServiceBase::~ServiceBase() {}

void WINAPI
ServiceBase::serviceMain(DWORD argc, PWSTR* argv)
{
    // Register the handler function for the service
    currentService_->statusHandle_ = RegisterServiceCtrlHandler(currentService_->serviceName_,
        serviceCtrlHandler);
    if (currentService_->statusHandle_ == NULL) {
        throw GetLastErrorAsPWSTR();
    }

    // Start the service.
    currentService_->serviceStart(argc, argv);
}

void
ServiceBase::setServiceStatus(DWORD currentState,
    DWORD win32ExitCode,
    DWORD waitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure of the service.
    serviceStatus_.dwCurrentState = currentState;
    serviceStatus_.dwWin32ExitCode = win32ExitCode;
    serviceStatus_.dwWaitHint = waitHint;

    /*
    * The Service Control Manager uses the dwWaitHint and dwCheckpoint members
    * of the SERVICE_STATUS structure to determine how much time to wait for a
    * Windows Service to start or shut down. If your OnStart and OnStop methods
    * run long, your service can request more time by calling SetServiceStatus
    * again with an incremented dwCheckPoint value.
    */
    serviceStatus_.dwCheckPoint =
        ((currentState == SERVICE_RUNNING) ||
        (currentState == SERVICE_STOPPED)) ?
        0 : dwCheckPoint++;

    // Report the status of the service to the SCM.
    if (!SetServiceStatus(statusHandle_, &serviceStatus_)) {
        throw GetLastErrorAsPWSTR();
    }
}

void WINAPI
ServiceBase::serviceCtrlHandler(DWORD ctrl)
{
    /** ctrl - the control code. This parameter can be one of the
     *   following values:
     *
     *     SERVICE_CONTROL_CONTINUE
     *     SERVICE_CONTROL_INTERROGATE
     *     SERVICE_CONTROL_NETBINDADD
     *     SERVICE_CONTROL_NETBINDDISABLE
     *     SERVICE_CONTROL_NETBINDREMOVE
     *     SERVICE_CONTROL_PARAMCHANGE
     *     SERVICE_CONTROL_PAUSE
     *     SERVICE_CONTROL_SHUTDOWN
     *     SERVICE_CONTROL_STOP
     *
     * This parameter can also be a user-defined control code ranges from 128
     * to 255.

     * The Service Control Handler was registered in your Service Main Entry point.
     * Each service must have a handler to handle control requests from the SCM.
     * The control handler must return within 30 seconds or the SCM will return an error
     * stating that the service is not responding. This is because the handler will be called in
     * the context of the SCM and will hold the SCM until it returns from the handler.
     */
    switch (ctrl)
    {
    case SERVICE_CONTROL_STOP: currentService_->serviceStop(); break;
    case SERVICE_CONTROL_PAUSE: currentService_->servicePause(); break;
    case SERVICE_CONTROL_CONTINUE: currentService_->serviceContinue(); break;
    case SERVICE_CONTROL_SHUTDOWN: currentService_->serviceShutdown(); break;
    case SERVICE_CONTROL_INTERROGATE: break;
    default: break;
    }
}

void
ServiceBase::serviceStart(DWORD argc, PWSTR * argv)
{
    try {
        // Tell scm that the service is about to start.
        setServiceStatus(SERVICE_START_PENDING);

        // Perform service-specific initialization.
        onStartCallback(argc, argv);

        // Tell SCM that the service is started.
        setServiceStatus(SERVICE_RUNNING);

    }
    catch (PWSTR dwError) {
        // Log the error.
        writeEventLogEntry(dwError, EVENTLOG_ERROR_TYPE);

        // Set the service status to be stopped.
        setServiceStatus(SERVICE_STOPPED, GetLastError());
    }
}

void
ServiceBase::onStartCallback(DWORD argc, PWSTR* argv)
{
}

void
ServiceBase::serviceStop()
{
    DWORD dwOriginalState = serviceStatus_.dwCurrentState;
    try {
        // Tell SCM that the service is stopping.
        setServiceStatus(SERVICE_STOP_PENDING);

        // Perform service-specific stop operations.
        onStopCallback();

        // Tell SCM that the service is stopped.
        setServiceStatus(SERVICE_STOPPED);

    }
    catch (PWSTR dwError) {
        // Log the error.
        writeEventLogEntry(dwError, EVENTLOG_ERROR_TYPE);

        // Set the orginal service status.
        setServiceStatus(dwOriginalState);
    }
}

void
ServiceBase::onStopCallback()
{
}

void ServiceBase::servicePause()
{
    try {
        // Tell SCM that the service is pausing.
        setServiceStatus(SERVICE_PAUSE_PENDING);

        // Perform service-specific pause operations.
        onPauseCallback();

        // Tell SCM that the service is paused.
        setServiceStatus(SERVICE_PAUSED);

    }
    catch (PWSTR dwError) {
        // Log the error.
        writeEventLogEntry(dwError, EVENTLOG_ERROR_TYPE);

        // Tell SCM that the service is still running.
        setServiceStatus(SERVICE_RUNNING);
    }
}

void
ServiceBase::onPauseCallback()
{
}

void
ServiceBase::serviceContinue()
{
    try {
        // Tell SCM that the service is resuming.
        setServiceStatus(SERVICE_CONTINUE_PENDING);

        // Perform service-specific continue operations.
        onContinueCallback();

        // Tell SCM that the service is running.
        setServiceStatus(SERVICE_RUNNING);

    }
    catch (PWSTR dwError) {
        // Log the error.
        writeEventLogEntry(dwError, EVENTLOG_ERROR_TYPE);

        // Tell SCM that the service is still paused.
        setServiceStatus(SERVICE_PAUSED);
    }
}

void
ServiceBase::onContinueCallback()
{
}

void ServiceBase::serviceShutdown()
{
    try {
        // Perform service-specific shutdown operations.
        onShutdownCallback();

        // Tell SCM that the service is stopped.
        setServiceStatus(SERVICE_STOPPED);

    }
    catch (PWSTR dwError) {
        // Log the error.
        writeEventLogEntry(dwError, EVENTLOG_ERROR_TYPE);
    }
}

void
ServiceBase::onShutdownCallback()
{
}

PWSTR
ServiceBase::GetLastErrorAsPWSTR()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return PWSTR(); //No error message has been recorded

    WCHAR *ptr, *msg;

    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPWSTR)&msg, 0/*sz*/, NULL);

    // Get rid of the CR and LF that FormatMessage() sticks at the
    // end of the message. Thanks Microsoft!
    ptr = msg;
    while (*ptr) {
        if (*ptr == '\r') {
            *ptr = 0x0000;
            break;
        }
        ptr++;
    }

    return msg;
}

void
ServiceBase::writeEventLogEntry(PWSTR message, WORD type)
{
    HANDLE hEventSource = NULL;
    LPCWSTR lpszStrings[2] = { NULL, NULL };

    hEventSource = RegisterEventSource(NULL, serviceName_);
    if (hEventSource)
    {
        lpszStrings[0] = serviceName_;
        lpszStrings[1] = message;

        ReportEvent(hEventSource,  // Event log handle
            type,                 // Event type
            0,                     // Event category
            0,                     // Event identifier
            NULL,                  // No security identifier
            2,                     // Size of lpszStrings array
            0,                     // No binary data
            lpszStrings,           // Array of strings
            NULL                   // No binary data
        );

        DeregisterEventSource(hEventSource);
    }
}

BOOL
ServiceBase::findProcessHandle(std::wstring& processName, HANDLE& hProcess)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        writeEventLogEntry(L"Error when taking process snapshot", EVENTLOG_ERROR_TYPE);
        return FALSE;
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process, and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        writeEventLogEntry(L"Error when finding the first process", EVENTLOG_ERROR_TYPE);
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return FALSE;
    }

    // Now walk the snapshot of processes, and display information about each process in turn
    do {
        // Compare whether the process is the process that we want to find

        std::wstring current_process_name{ std::wstring(pe32.szExeFile) };
        if (current_process_name.compare(processName) != 0) {
            continue;
        }

        // Retrieve the priority class.
        dwPriorityClass = 0;
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
        if (hProcess == NULL) {
            writeEventLogEntry(L"Error when openning process", EVENTLOG_ERROR_TYPE);
        }
        //CloseHandle(hProcess);
        CloseHandle(hProcessSnap);
        return TRUE;

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return FALSE;
}
