
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

import "../js/videodevicecontextmenuitemcreation.js" as VideoDeviceContextMenuItemCreation
import "../js/selectscreenwindowcreation.js" as SelectScreenWindowCreation
import "../js/screenrubberbandcreation.js" as ScreenRubberBandCreation

Menu {
    id: contextMenu

    property string responsibleAccountId: ""
    property string responsibleConvUid: ""

    property int generalMenuSeparatorCount: 0
    property int commonBorderWidth: 2

    signal fullScreenNeeded

    function activate() {
        AvAdapter.videoContextMenuForDeviceItemCreationHelper()
    }

    function closePotentialWindows() {
        SelectScreenWindowCreation.destorySelectScreenWindow()
        ScreenRubberBandCreation.destoryScreenRubberBandWindow()
    }

    JamiFileDialog {
        id: jamiFileDialog

        selectMultiple: true
        selectExisting: true

        onAccepted: {
            var filePaths = jamiFileDialog.fileUrls


            /*
             * No need to trim file:///.
             */
            AvAdapter.shareFile(filePaths[0])
        }
    }

    Connections {
        target: AvAdapter


        /*
         * Dynamatic creation of device menu item
         */
        onNewVideoContextMenuForDeviceItem: {
            if (videoDeviceItem.itemName === "No video device") {
                videoDeviceItem.checkable = true
                videoDeviceItem.itemName = deviceName
                videoDeviceItem.checked = setChecked
                if (last)
                    contextMenu.open()
            } else {
                VideoDeviceContextMenuItemCreation.createVideoDeviceContextMenuItemObjects(
                            deviceName, setChecked, last)
            }
        }
    }


    /*
     * All GeneralMenuItems should remain the same width / height.
     * The first videoDeviceItem is to make sure the border is correct.
     */
    VideoCallPageContextMenuDeviceItem {
        id: videoDeviceItem

        topBorderWidth: commonBorderWidth
    }

    GeneralMenuSeparator {
        preferredWidth: videoDeviceItem.preferredWidth
        preferredHeight: commonBorderWidth

        Component.onCompleted: {
            generalMenuSeparatorCount++
        }
    }

    GeneralMenuItem {
        id: shareEntireScreenItem

        itemName: qsTr("Share entire screen")
        leftBorderWidth: commonBorderWidth
        rightBorderWidth: commonBorderWidth

        onClicked: {
            contextMenu.close()
            if (Qt.application.screens.length === 1) {
                AvAdapter.shareEntireScreen(0)
            } else {
                SelectScreenWindowCreation.createSelectScreenWindowObject()
                SelectScreenWindowCreation.showSelectScreenWindow()
            }
        }
    }

    GeneralMenuItem {
        id: shareScreenAreaItem

        itemName: qsTr("Share screen area")
        leftBorderWidth: commonBorderWidth
        rightBorderWidth: commonBorderWidth

        onClicked: {
            contextMenu.close()
            if (Qt.application.screens.length === 1) {
                ScreenRubberBandCreation.createScreenRubberBandWindowObject(
                            null, 0)
                ScreenRubberBandCreation.showScreenRubberBandWindow()
            } else {
                SelectScreenWindowCreation.createSelectScreenWindowObject(true)
                SelectScreenWindowCreation.showSelectScreenWindow()
            }
        }
    }

    GeneralMenuItem {
        id: shareFileItem

        itemName: qsTr("Share file")
        leftBorderWidth: commonBorderWidth
        rightBorderWidth: commonBorderWidth

        onClicked: {
            contextMenu.close()
            jamiFileDialog.open()
        }
    }

    GeneralMenuSeparator {
        preferredWidth: videoDeviceItem.preferredWidth
        preferredHeight: commonBorderWidth

        Component.onCompleted: {
            generalMenuSeparatorCount++
        }
    }

    GeneralMenuItem {
        id: fullScreenItem

        property bool isFullScreen: false

        itemName: isFullScreen ? qsTr("Exit full screen") : qsTr(
                                     "Full screen mode")
        iconSource: isFullScreen ? "qrc:/images/icons/ic_exit_full_screen_black.png" : "qrc:/images/icons/ic_full_screen_black.png"
        leftBorderWidth: commonBorderWidth
        rightBorderWidth: commonBorderWidth
        bottomBorderWidth: commonBorderWidth

        onClicked: {
            contextMenu.close()
            contextMenu.fullScreenNeeded()
            isFullScreen = !isFullScreen
        }
    }

    onClosed: {
        videoDeviceItem.itemName = "No video device"
        VideoDeviceContextMenuItemCreation.removeCreatedItems()
    }

    Component.onCompleted: {
        VideoDeviceContextMenuItemCreation.setVideoContextMenuObject(
                    contextMenu)
    }

    background: Rectangle {
        implicitWidth: videoDeviceItem.preferredWidth
        implicitHeight: videoDeviceItem.preferredHeight
                        * (contextMenu.count - generalMenuSeparatorCount)

        border.width: commonBorderWidth
        border.color: JamiTheme.tabbarBorderColor
    }
}
