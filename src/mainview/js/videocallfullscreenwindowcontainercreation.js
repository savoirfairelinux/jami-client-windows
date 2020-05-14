
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
 * Global video call full screen window container, object variable for creation.
 */
var videoCallFullScreenWindowContainerComponent
var videoCallFullScreenWindowContainerObject

function createvideoCallFullScreenWindowContainerObject() {
    if (videoCallFullScreenWindowContainerObject)
        return
    videoCallFullScreenWindowContainerComponent = Qt.createComponent(
                "../components/VideoCallFullScreenWindowContainer.qml")
    if (videoCallFullScreenWindowContainerComponent.status === Component.Ready)
        finishCreation()
    else if (videoCallFullScreenWindowContainerComponent.status === Component.Error)
        console.log("Error loading component:",
                    videoCallFullScreenWindowContainerComponent.errorString())
}

function finishCreation() {
    videoCallFullScreenWindowContainerObject
            = videoCallFullScreenWindowContainerComponent.createObject()
    if (videoCallFullScreenWindowContainerObject === null) {


        /*
         * Error Handling.
         */
        console.log("Error creating video call full screen window container object")
    }


    /*
     * Signal connection.
     */
    videoCallFullScreenWindowContainerObject.onClosing.connect(
                destoryVideoCallFullScreenWindowContainer)
}


/*
 * Destroy and reset videoCallFullScreenWindowContainerObject when window is closed.
 */
function destoryVideoCallFullScreenWindowContainer() {
    if (!videoCallFullScreenWindowContainerObject)
        return
    videoCallFullScreenWindowContainerObject.destroy()
    videoCallFullScreenWindowContainerObject = false
}


/*
 * Need to be used after createvideoCallFullScreenWindowContainerObject().
 */
function getObject() {
    return videoCallFullScreenWindowContainerObject
}
