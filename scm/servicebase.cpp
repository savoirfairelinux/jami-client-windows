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

#include <winsvc.h>

// Initialize service instance.
ServiceBase* ServiceBase::currentService_ = NULL;

BOOL
ServiceBase::scmRegister(ServiceBase &service)
{
    // assign created instance to current service
    // other functions are triggered by this instance
    currentService_ = &service;

    if(currentService_ == NULL)
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
                         BOOL canPauseContinue):
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
        throw GetLastErrorAsWstring();
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
        throw GetLastErrorAsWstring();
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

    } catch (std::wstring dwError) {
        // Log the error.
        printf("Service Start: %ls \n", dwError.c_str());

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

    } catch (std::wstring dwError) {
        // Log the error.
        printf("Service Stop: %ls \n", dwError.c_str());

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

    } catch (std::wstring dwError) {
        // Log the error.
        printf("Service Pause: %ls \n", dwError.c_str());

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

    } catch (std::wstring dwError) {
        // Log the error.
        printf("Service Continue: %ls \n", dwError.c_str());

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

    } catch (std::wstring dwError) {
        // Log the error.
        printf("Service Shutdown: %ls \n", dwError.c_str());
    }
}

void
ServiceBase::onShutdownCallback()
{
}

std::wstring
ServiceBase::GetLastErrorAsWstring()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::wstring(); //No error message has been recorded

    wchar_t buf[256];
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

    std::wstring message(buf);

    return message;
}

