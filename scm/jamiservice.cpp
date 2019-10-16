#include "jamiservice.h"

JamiProcessMonitor::JamiProcessMonitor(PWSTR serviceName,
                                       BOOL canStop,
                                       BOOL canShutdown,
                                       BOOL canPauseContinue) :
    ServiceBase(serviceName, canStop, canShutdown, canPauseContinue),
    stopping_(FALSE),
    killProcessEvent_(CreateEvent(NULL, TRUE, FALSE, NULL))
{
    // Create a manual-reset event that is not signaled at first to indicate
    // the stopped signal of the service.
    // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventa

    if (killProcessEvent_ == NULL) {
        throw GetLastErrorAsWstring();
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
    printf("service running...\n");
    serviceThread_ = CreateThread(0, 0,
                     (LPTHREAD_START_ROUTINE)serviceWorkerThread,
                     (void*)this, 0, &id);
    if (serviceThread_ == NULL) {
        throw GetLastErrorAsWstring();
    } else {
        stopping_ = FALSE;
    }
}

void
JamiProcessMonitor::onStopCallback()
{
    // Log a service stop message to the Application log.
    printf("service stopped \n");

    // Indicate that the service is stopping and wait for the finish of the
    // main service function (ServiceWorkerThread).

    stopping_ = TRUE;
    if (WaitForSingleObject(killProcessEvent_, INFINITE) != WAIT_OBJECT_0) {
        throw GetLastErrorAsWstring();
    }
}
DWORD
JamiProcessMonitor::serviceWorkerThread(void* param)
{
    JamiProcessMonitor* currentInstance = (JamiProcessMonitor*)param;
    while (!currentInstance->stopping_) {
        Beep(450, 150);
        Sleep(4000);
    }

    // Signal the stopped event.
    SetEvent(currentInstance->killProcessEvent_);

    return 0;
}
