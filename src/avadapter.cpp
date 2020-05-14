/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author : Edric Ladent Milaret<edric.ladent - milaret @savoirfairelinux.com>
 * Author : Andreas Traczyk<andreas.traczyk @savoirfairelinux.com>
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "avadapter.h"

#include "lrcinstance.h"

#include <QScreen>

AvAdapter::AvAdapter(QObject *parent)
    : QmlAdapterBase(parent)
{}

AvAdapter::~AvAdapter() {}

void
AvAdapter::initQmlObject()
{}

void
AvAdapter::videoContextMenuForDeviceItemCreationHelper()
{
    auto conversation = LRCInstance::getCurrentConversation();
    auto call = LRCInstance::getCallInfoForConversation(conversation);
    if (!call) {
        return;
    }

    auto activeDevice = LRCInstance::avModel().getCurrentRenderedDevice(call->id);

    /*
     * Create a list of video input devices.
     */
    auto devices = LRCInstance::avModel().getDevices();
    for (int i = 0; i < devices.size(); i++) {
        try {
            auto settings = LRCInstance::avModel().getDeviceSettings(devices[i]);
            emit newVideoContextMenuForDeviceItem(settings.name,
                                                  devices[i] == activeDevice.name,
                                                  i == devices.size() - 1);
        } catch (...) {
            qDebug().noquote() << "Error in getting device settings";
        }
    }
}

void
AvAdapter::videoContextMenuForDeviceItemOnClickedSlot(const QString &deviceName)
{
    auto deviceId = LRCInstance::avModel().getDeviceIdFromName(deviceName);
    if (deviceId.isEmpty()) {
        qWarning() << "Couldn't find device: " << deviceName;
        return;
    }
    LRCInstance::avModel().switchInputTo(deviceId);
    LRCInstance::avModel().setCurrentVideoCaptureDevice(deviceId);
}

void
AvAdapter::shareEntireScreen(int screenNumber)
{
    QScreen *screen = qApp->screens().at(screenNumber);
    if (!screen)
        return;
    QRect rect = screen ? screen->geometry() : qApp->primaryScreen()->geometry();
    LRCInstance::avModel().setDisplay(screenNumber, rect.x(), rect.y(), rect.width(), rect.height());
}

const QString
AvAdapter::shootScreen(int screenNumber)
{
    QScreen *screen = qApp->screens().at(screenNumber);
    if (!screen)
        return QString("");
    /*
     * The screen window id is always 0.
     */
    auto pixmap = screen->grabWindow(0);

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    return QString::fromLatin1(buffer.data().toBase64().data());
}
