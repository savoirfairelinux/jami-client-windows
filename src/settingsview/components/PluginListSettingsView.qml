/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
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

import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import Qt.labs.platform 1.1
import QtGraphicalEffects 1.14
import net.jami.Models 1.0
import "../../commoncomponents"

Rectangle {
    id: pluginListSettingsViewRect

    function updatePluginListDisplayed() {
        // settings
    }

    function openPluginFileSlot(){
        pluginPathDialog.open()
    }

    function uninstallPluginSlot(pluginId){
        uninstallPluginMessageBox.pluginId = pluginId
        uninstallPluginMessageBox.open()
    }

    function uninstallPlugin(pluginId){
        ClientWrapper.pluginModel.uninstallPlugin(pluginId)
        updateAndShowPluginsSlot()
    }

    function updateAndShowPluginsSlot()
    {
        pluginItemListModel.reset()
    }

    function loadPluginSlot(pluginId, isLoaded){
        if (isLoaded)
            ClientWrapper.pluginModel.loadPlugin(pluginId)
        else
            ClientWrapper.pluginModel.unloadPlugin(pluginId)
    }

    JamiFileDialog {
        id: pluginPathDialog

        mode: JamiFileDialog.OpenFile
        title: qsTr("Select A Plugin to Install")
        folder: StandardPaths.writableLocation(StandardPaths.DownloadLocation)

        nameFilters: [qsTr("Plugin Files") + " (*.jpl)", qsTr(
                "All files") + " (*)"]

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }

        onAccepted: {
            var url = ClientWrapper.utilsAdaptor.getAbsPath(file.toString())
            ClientWrapper.pluginModel.installPlugin(url, true)
            updateAndShowPluginsSlot()
        }
    }

    MessageBox{
        id: uninstallPluginMessageBox

        property string pluginId: ""

        title:qsTr("Uninstall plugin")
        text :qsTr("Are you sure you wish to uninstall this plugin?")
        // icon :StandardIcon.Information
        standardButtons: StandardButton.Ok | StandardButton.Cancel

        onYes: {
            accepted()
        }

        onNo:{
            rejected()
        }

        onDiscard: {
            rejected()
        }

        onAccepted: {
            uninstallPlugin(pluginId)
        }

        onRejected: {}
    }
    PluginItemListModel {
        id: pluginItemListModel
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        Layout.fillHeight: true
        Layout.maximumWidth: 580
        Layout.preferredWidth: 580
        Layout.minimumWidth: 580

        Label {
            Layout.fillWidth: true
            Layout.minimumHeight: 25
            Layout.preferredHeight: 25
            Layout.maximumHeight: 25

            text: qsTr("Installed plugins")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true
            Layout.minimumHeight: 24
            Layout.preferredHeight: 24
            Layout.maximumHeight: 24
        }

        ColumnLayout {
            spacing: 7

            Layout.fillWidth: true


            HoverableRadiusButton {
                id: downloadButton

                Layout.maximumWidth: 320
                Layout.preferredWidth: 320
                Layout.minimumWidth: 320

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                radius: height / 2

                text: qsTr("+ Install plugin")
                fontPointSize: 10
                font.kerning: true

                onClicked: {
                    openPluginFileSlot()
                }
            }   

            ListViewJami {
                id: pluginListView

                Layout.leftMargin: 20

                Layout.fillWidth: true

                Layout.minimumWidth: 320
                Layout.preferredWidth: 320

                Layout.minimumHeight: 164
                Layout.preferredHeight: 164
                Layout.maximumHeight: 164

                model: pluginItemListModel

                delegate: PluginItemDelegate{
                    id: pluginItemDelegate

                    width: pluginListView.width
                    height: 80

                    pluginName : PluginName
                    pluginId: PluginId
                    pluginIcon: PluginIcon
                    isLoaded: IsLoaded

                    onClicked: {
                        pluginListView.currentIndex = index
                    }

                    onBtnUninstallPluginClicked:{        
                        uninstallPluginSlot(pluginId)
                    }

                    onBtnLoadPluginToggled:{
                        loadPluginSlot(pluginId, isLoaded)
                    }
                }
            }
        }

        // spacer on the bottom
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }
}



                    