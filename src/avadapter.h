/*
 * Copyright (C) 2020 by Savoir-faire Linux
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

#pragma once

#include "qmladapterbase.h"

#include <QObject>
#include <QString>

class AvAdapter : public QmlAdapterBase
{
    Q_OBJECT

public:
    explicit AvAdapter(QObject *parent = nullptr);
    ~AvAdapter();

    Q_INVOKABLE void initQmlObject() override;
    Q_INVOKABLE void videoContextMenuForDeviceItemCreationHelper();

    /*
     * Preview video input switching.
     */
    Q_INVOKABLE void videoContextMenuForDeviceItemOnClickedSlot(const QString &deviceName);

    /*
     * Share the screen specificed by screen number.
     */
    Q_INVOKABLE void shareEntireScreen(int screenNumber);

    /*
     * Take snap shoot of the screen.
     */
    Q_INVOKABLE const QString shootScreen(int screenNumber);

    /*
     * Share a media file.
     */
    Q_INVOKABLE void shareFile(const QString &filePath);

    /*
     * Select screen area to display.
     */
    Q_INVOKABLE void displayArea(int screenNumber, int x, int y, int width, int height);

signals:
    void newVideoContextMenuForDeviceItem(const QString &deviceName, bool setChecked, bool last);
};
