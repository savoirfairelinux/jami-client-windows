
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.14
import QtQuick.Controls 2.14
import net.jami.JamiTheme 1.0
import net.jami.AvAdapter 1.0

import "../../commoncomponents"


/*
 * Take advantage of child can access parent's item (ex: contextMenu, commonBorderWidth).
 */
GeneralMenuItem {
    id: videoCallPageContextMenuDeviceItem

    itemName: qsTr("No video device")
    leftBorderWidth: commonBorderWidth
    rightBorderWidth: commonBorderWidth

    contentItem: Text {
        leftPadding: videoCallPageContextMenuDeviceItem.indicator.width / 2 + 10
        rightPadding: videoCallPageContextMenuDeviceItem.arrow.width
        text: videoCallPageContextMenuDeviceItem.itemName
        font: videoCallPageContextMenuDeviceItem.font
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }

    indicator: Item {
        id: selectItem

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter

        implicitWidth: 32
        implicitHeight: 32

        Rectangle {
            id: selectRect

            width: selectItem.width / 2
            height: selectItem.height / 2
            anchors.centerIn: parent
            visible: videoCallPageContextMenuDeviceItem.checkable
            border.color: JamiTheme.selectionGreen
            radius: 3
            Rectangle {
                width: selectRect.width / 2
                height: selectRect.height / 2
                anchors.centerIn: parent
                visible: videoCallPageContextMenuDeviceItem.checked
                color: JamiTheme.selectionGreen
                radius: 2
            }
        }
    }

    onClicked: {
        var deviceName = videoCallPageContextMenuDeviceItem.itemName
        contextMenu.close()
        AvAdapter.videoContextMenuForDeviceItemOnClickedSlot(deviceName)
    }
}
