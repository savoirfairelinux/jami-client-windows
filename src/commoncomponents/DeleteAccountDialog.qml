/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Yang Wang <yang.wang@savoirfairelinux.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.Models 1.0

Dialog {
    id: deleteAccountDialog

    visible: false

    implicitWidth: 400
    implicitHeight: 300

    ColumnLayout{
        anchors.fill: parent
        spacing: 7

        Label{
            id: labelDeletion
        }

        Label{
            id: labelBestId
        }

        Label{
            id: labelAccountHash
        }

        Item{
            Layout.fillWidth: true

            Layout.maximumHeight: 5
            Layout.preferredHeight: 5
            Layout.minimumHeight: 5
        }

        Label{
            id: labelWarning
        }

        Item{
            Layout.fillWidth: true

            Layout.maximumHeight: 10
            Layout.preferredHeight: 10
            Layout.minimumHeight: 10
        }

        RowLayout{
            spacing: 7
        }

        Item{
            Layout.fillWidth: true

            Layout.maximumHeight: 5
            Layout.preferredHeight: 5
            Layout.minimumHeight: 5
        }
    }
}
