
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
import QtQuick.Layouts 1.14
import net.jami.Models 1.0


/*
 * General menu item.
 * Can control top, bottom, left, right border width.
 * Use onClicked slot to simulate item click event.
 * Can have image icon at the left of the text.
 */
MenuItem {
    id: menuItem

    property string itemName: ""
    property string iconSource: ""
    property int preferredWidth: 150
    property int preferredHeight: 30
    property int topBorderWidth: 0
    property int bottomBorderWidth: 0
    property int leftBorderWidth: 0
    property int rightBorderWidth: 0

    signal clicked

    contentItem: Rectangle {
        id: menuItemContentRect

        anchors.fill: parent

        RowLayout {
            id: menuItemContentRectRowLayout

            anchors.fill: menuItemContentRect

            Image {
                id: contextMenuItemImage

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: 5
                Layout.preferredWidth: 25
                Layout.preferredHeight: 25

                visible: false
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }

            Text {
                id: contextMenuItemText

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                Layout.leftMargin: contextMenuItemImage.visible ? 0 : 5
                Layout.preferredWidth: contextMenuItemImage.visible ? (preferredWidth - contextMenuItemImage.width - 5) : preferredWidth
                Layout.preferredHeight: 30

                text: itemName
                font.pointSize: JamiTheme.textFontSize - 3
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
        color: "transparent"
    }

    onIconSourceChanged: {
        if (iconSource !== "") {
            contextMenuItemImage.source = iconSource
            contextMenuItemImage.visible = true
        }
    }

    background: Rectangle {
        id: contextMenuBackgroundRect

        anchors.fill: parent
        anchors.topMargin: topBorderWidth
        anchors.bottomMargin: bottomBorderWidth
        anchors.leftMargin: leftBorderWidth
        anchors.rightMargin: rightBorderWidth

        implicitWidth: preferredWidth
        implicitHeight: preferredHeight

        border.width: 0
        color: menuItem.down ? JamiTheme.releaseColor : "white"

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onPressed: {
                contextMenuBackgroundRect.color = JamiTheme.pressColor
            }
            onReleased: {
                contextMenuBackgroundRect.color = JamiTheme.releaseColor
                menuItem.clicked()
            }
            onEntered: {
                contextMenuBackgroundRect.color = JamiTheme.hoverColor
            }
            onExited: {
                contextMenuBackgroundRect.color = "white"
            }
        }

        CustomBorder {
            commonBorder: false
            lBorderwidth: leftBorderWidth
            rBorderwidth: rightBorderWidth
            tBorderwidth: topBorderWidth
            bBorderwidth: bottomBorderWidth
            borderColor: JamiTheme.tabbarBorderColor
        }
    }
}
