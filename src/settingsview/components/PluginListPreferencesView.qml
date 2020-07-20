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
    id: pluginListPreferencesViewRect

    signal updatePluginList

    property string pluginName: ""
    property string pluginIcon: ""
    property string pluginId: ""

    visible: false

    function updatePreferenceListDisplayed(){
        // settings
    }

    function resetPluginSlot(){
        resetPluginMessageBox.open()
    }

    function resetPlugin(){
        ClientWrapper.pluginModel.resetPluginPreferencesValues(pluginId)
        updatePreferenceListDisplayed()
    }

    function uninstallPluginSlot(){
        uninstallPluginMessageBox.open()
    }

    function uninstallPlugin(){
        ClientWrapper.pluginModel.uninstallPlugin(pluginId)
        updatePluginList()
    }

    MessageBox{
        id: uninstallPluginMessageBox

        title:qsTr("Uninstall plugin")
        text :qsTr("Are you sure you wish to uninstall "+ pluginName + " ?")
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
            uninstallPlugin()
            pluginListPreferencesViewRect.visible = false
        }

        onRejected: {}
    }

    MessageBox{
        id: resetPluginMessageBox

        title:qsTr("Reset preferences")
        text :qsTr("Are you sure you wish to reset "+ pluginName + " preferences?")

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
            resetPlugin()
        }

        onRejected: {}
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        Layout.fillHeight: true
        Layout.maximumWidth: 580
        Layout.preferredWidth: 580
        Layout.minimumWidth: 580

        Label{
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 7

            Layout.minimumWidth: 30
            Layout.preferredWidth: 30
            Layout.maximumWidth: 30

            Layout.minimumHeight: 30
            Layout.preferredHeight: 30
            Layout.maximumHeight: 30

            background: Rectangle{
                anchors.fill: parent
                Image {
                    anchors.fill: parent
                    source: "file:"+pluginIcon
                }
            }
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumHeight: 25
            Layout.preferredHeight: 25
            Layout.maximumHeight: 25

            text: qsTr(pluginName)
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumHeight: 25
            Layout.preferredHeight: 25
            Layout.maximumHeight: 25

            text: qsTr("preferences")
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

        RowLayout {
            spacing: 7

            Layout.fillHeight: true

            HoverableRadiusButton {
                id: resetButton

                Layout.maximumWidth: 160
                Layout.preferredWidth: 160
                Layout.minimumWidth: 160

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                radius: height / 2

                icon.source: "qrc:/images/icons/settings_backup_restore-black-18dp.svg"
                icon.height: 24
                icon.width: 24

                text: qsTr("Reset")
                fontPointSize: 10
                font.kerning: true

                onClicked: {
                    resetPluginSlot()
                }
            }

            HoverableRadiusButton {
                id: uninstallButton

                Layout.maximumWidth: 160
                Layout.preferredWidth: 160
                Layout.minimumWidth: 160

                Layout.minimumHeight: 30
                Layout.preferredHeight: 30
                Layout.maximumHeight: 30

                radius: height / 2

                icon.source: "qrc:/images/icons/ic_delete_black_18dp_2x.png"
                icon.height: 24
                icon.width: 24

                text: qsTr("Uninstall")
                fontPointSize: 10
                font.kerning: true

                onClicked: {
                    uninstallPluginSlot()
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
