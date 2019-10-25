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
#include "serviceinstall.h"

int main(int argc, char *argv[])
{

    PWSTR serviceName = L"Jami-Service";

    if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/')))) {
        if (strcmp("install", argv[1] + 1) == 0) {
            wchar_t path[MAX_PATH];
            // If the first parameter is NULL, GetModuleFileName retrieves the path of the executable file of the current process.
            if (GetModuleFileName(NULL, path, ARRAYSIZE(path)) == 0) {
                std::wcout << "GetModuleFileName failed: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
                return 0;
            }

            installService(serviceName,
                           L"Jami-Service (Bug Report)",
                           SERVICE_DEMAND_START,
                           NULL, NULL, NULL,
                           path);

        } else if (strcmp("remove", argv[1] + 1) == 0) {
            uninstallService(serviceName);
        }
    } else {
        printf("\nUsage:\n\n");
        printf(" -install  to install the service.\n");
        printf(" -remove   to remove the service.\n");

        JamiProcessMonitor service(serviceName);
        if (!ServiceBase::scmRegister(service)) {
            std::wcout << "Service failed to run: " << ServiceBase::GetLastErrorAsPWSTR() << std::endl;
        }
    }
    return 0;
}
