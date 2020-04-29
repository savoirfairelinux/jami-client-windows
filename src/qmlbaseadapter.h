/***************************************************************************
 * Copyright (C) 2020 by Savoir-faire Linux                                *
 * Author: Mingrui Zhang   <mingrui.zhang@savoirfairelinux.com>            *
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

#include <QObject>

/*################################################################################
# The main purpose of this class is to operate on qml objects,
# or provide api calls to qml objects that cannot be done directly
# in qml
################################################################################*/

class QmlBaseAdapter : public QObject
{
    Q_OBJECT
public:
    explicit QmlBaseAdapter(QObject *parent = nullptr);
    ~QmlBaseAdapter();

    // Must call Q_INVOKABLE so that this function can be used in QML, qml to c++
    Q_INVOKABLE virtual void declareObject(QObject *obj);

protected:
    virtual void objectSetUp() = 0;

    // Object pointer
    QObject *correspondingQmlObject_;
};
