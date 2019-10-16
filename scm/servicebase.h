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

#include <iostream>
#include <string>

#include <windows.h>
#include <winsvc.h>

class ServiceBase
{
public:

    /**
     * Register the executable for a service with scm.
     * After you call scmRegister(ServiceBase), the SCM issues a Start command,
     * which results in a call to the onStart method in the service. This
     * method blocks until the service has stopped.
     *
     * @param service - reference to ServiceBase or its derived classes
     */
    static BOOL scmRegister(ServiceBase &service);

    /**
     * Returns the last Win32 error, in wstring format.
     * Returns an empty string if there is no error.
     *
     * @return error string
     */
    static std::wstring GetLastErrorAsWstring();

    /**
     * Service base constructor
     *
     * @param serviceName - service name
     * @param canStop - whether the service can be stopped
     * @param canShutdown - wwhether the service can be notified when system shutdown occurs.
     * @param canPauseContinue - whether the service can be paused and continued
     */
    explicit ServiceBase(PWSTR serviceName,
                         BOOL canStop = TRUE,
                         BOOL canShutdown = TRUE,
                         BOOL canPauseContinue = TRUE);

    virtual ~ServiceBase();

    /**
     * Stop the service.
     */
    void serviceStop();

protected:
    /**
     * Executes when a Start command is sent to the service by the SCM
     * or when the operating system starts (for a service that starts automatically).
     *
     * @param argc - number of command line arguments
     * @param argv - str of command line arguments
     */
    virtual void onStartCallback(DWORD argc, PWSTR *argv);

    /**
     * Executes when a Stop command is sent to the service by the SCM.
     */
    virtual void onStopCallback();

    /**
     * Executes when a Pause command is sent to the service by the SCM.
     */
    virtual void onPauseCallback();

    /**
     * Executes when a Continue command is sent to the service by the SCM.
     */
    virtual void onContinueCallback();

    /**
     * Executes when a system shuts down is sent to the service by the SCM.
     */
    virtual void onShutdownCallback();

    /**
     * Set current service status and report the status to the SCM.
     *
     * @param currentState - current service state
     * @param win32ExitCode - exit code
     * @param waitHint - estimated time for pending operation, in milliseconds
     */
    void setServiceStatus(DWORD currentState,
                          DWORD win32ExitCode = NO_ERROR,
                          DWORD waitHint = 0);

private:

    /**
     * Entry point for the service. It registers the handler function for the
     * service and starts the service.
     *
     * @param argc - number of command line arguments
     * @param argv - str of command line arguments
     */
    static void WINAPI serviceMain(DWORD argc, LPWSTR* argv);

    /**
     * The function is called by the SCM whenever a control code is sent to
     * the service.
     *
     * @param ctrl - control code
     */
    static void WINAPI serviceCtrlHandler(DWORD ctrl);

    /**
     * Start the service.
     *
     * @param argc - number of command line arguments
     * @param argv - str of command line arguments
     */
    void serviceStart(DWORD argc, PWSTR *argv);

    // Pause the service.
    void servicePause();

    // Resume the service after being paused.
    void serviceContinue();

    // Execute when the system is shutting down.
    void serviceShutdown();

    // The name of the service
    PWSTR serviceName_;

    // The status of the service
    SERVICE_STATUS serviceStatus_;

    // The service status handle
    SERVICE_STATUS_HANDLE statusHandle_;

    // The singleton service instance as
    // a pointer point to itself
    static ServiceBase* currentService_;
};
