
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


/*
 * Global storage for created videoContextMenuItem,
 * will be cleared once context menu is closed.
 */
var itemArray = []


/*
 * Global videoContextMenu component, object variable for creation.
 */
var videoContextMenuObject
var videoContextMenuItemComponent
var videoContextMenuItemObject


/*
 * Init videoContextMenuObject.
 */
function setVideoContextMenuObject(obj) {
    videoContextMenuObject = obj
}

function createVideoContextMenuObjects(deviceName, setChecked, last) {

    videoContextMenuItemComponent = Qt.createComponent(
                "../components/VideoCallPageContextMenuDeviceItem.qml")
    if (videoContextMenuItemComponent.status === Component.Ready)
        finishCreation(deviceName, setChecked, last)
    else if (videoContextMenuItemComponent.status === Component.Error)
        console.log("Error loading component:",
                    videoContextMenuItemComponent.errorString())
}

function finishCreation(deviceName, setChecked, last) {
    videoContextMenuItemObject = videoContextMenuItemComponent.createObject()
    if (videoContextMenuItemObject === null) {


        /*
         * Error Handling.
         */
        console.log("Error creating video context menu object")
    }

    videoContextMenuItemObject.itemName = deviceName
    videoContextMenuItemObject.checkable = true
    videoContextMenuItemObject.checked = setChecked


    /*
     * Push into the storage array, and insert it into context menu.
     */
    itemArray.push(videoContextMenuItemObject)
    videoContextMenuObject.insertItem(1, videoContextMenuItemObject)


    /*
     * If it is the last device context menu item, open the context menu.
     */
    if (last)
        videoContextMenuObject.open()
}

function removeCreatedItems() {
    var arrayLength = itemArray.length
    for (var i = 0; i < arrayLength; i++) {
        videoContextMenuObject.removeItem(itemArray[i])
        itemArray[i].destroy()
    }
    itemArray = []
}
