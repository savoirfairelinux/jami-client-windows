/***************************************************************************
 * Copyright (C) 2017 by Savoir-faire Linux                                *
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

#include "shmclient.h"

#include <QTextStream>

ShmClient::ShmClient(QSharedMemory* shm, QSystemSemaphore *sem) :
    shm_(shm),
    sem_(sem)
{
    start();
}

void ShmClient::run()
{
    while (true) {

        sem_->acquire();

        shm_->lock();

        char const *from = (char const*)shm_->data();

        QString uri = "";
        QTextStream stream(&uri);

        while (from && *from)
        {
            stream << *from;
            ++from;
        }

        shm_->unlock();

        emit RingEvent(uri);
    }
}
