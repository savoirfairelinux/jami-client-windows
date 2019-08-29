/***************************************************************************
 * Copyright (C) 2015-2019 by Savoir-faire Linux                           *
 * Author: Edric Ladent Milaret <edric.ladent-milaret@savoirfairelinux.com>*
 * Author: Andreas Traczyk <andreas.traczyk@savoirfairelinux.com>          *
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

#include <QDialog>
#include <QSortFilterProxyModel>

#include "lrcinstance.h"
#include "utils.h"

namespace Ui {
class DiscoveredPeerPicker;
}

class DiscoveredPeerPicker : public QDialog {
    Q_OBJECT;

public:

    explicit DiscoveredPeerPicker(QWidget* parent = 0);
    ~DiscoveredPeerPicker();

protected:
    void mousePressEvent(QMouseEvent* event);

signals:
    void willClose(QMouseEvent* event);

private:
    Ui::DiscoveredPeerPicker* ui;

    QString CalleeDisplayName_;
};
