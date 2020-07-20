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
    id: pluginSettingsRect

    function populatePluginSettings(){
        // settings
        enabledplugin.checked = ClientWrapper.pluginModel.getPluginsEnabled()
    }

    function slotSetPluginEnabled(state){
        ClientWrapper.pluginModel.setPluginsEnabled(state)
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    ScrollView{
        anchors.fill: parent
        clip: true

        RowLayout {
            width: pluginSettingsRect.width
            height: pluginSettingsRect.height

            spacing: 0

            Item {
                Layout.fillHeight: true
                Layout.maximumWidth: 48
                Layout.preferredWidth: 48
                Layout.minimumWidth: 48
            }

            ColumnLayout {
                spacing: 6

                Layout.fillHeight: true
                Layout.maximumWidth: 580
                Layout.preferredWidth: 580
                Layout.minimumWidth: 580

                Item {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 25
                    Layout.preferredHeight: 25
                    Layout.maximumHeight: 25

                    text: qsTr("Plugin")
                    font.pointSize: 15
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

                // system setting panel
                ColumnLayout {
                    spacing: 6
                    Layout.fillWidth: true


                    ToggleSwitch {
                        id: enabledplugin

                        Layout.leftMargin: 20

                        labelText: "Enable plugins"
                        fontPointSize: 11

                        onSwitchToggled: {
                            slotSetPluginEnabled(checked)
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
    }
}
