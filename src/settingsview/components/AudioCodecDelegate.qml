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
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.Models 1.0

ItemDelegate {
    id: videoCodecDelegate

    property string audioCodecName : ""
    property bool isEnabled : false
    property int audioCodecId
    property string samplerRate: ""

    signal audioCodecStateChange(string idToSet , bool isToBeEnabled)

    property int checkBoxWidth: 10

    highlighted: ListView.isCurrentItem

    RowLayout{
        anchors.fill: parent

        spacing: 10

        CheckBox{
            id: checkBoxIsEnabled

            Layout.leftMargin: 20

            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            Layout.fillHeight: true

            Layout.minimumWidth: checkBoxWidth
            Layout.preferredWidth: checkBoxWidth
            Layout.maximumWidth: checkBoxWidth

            tristate: false
            checkState: isEnabled ? Qt.Checked : Qt.Unchecked

            indicator.implicitWidth: checkBoxWidth
            indicator.implicitHeight:checkBoxWidth

            indicator.layer.textureSize.width: checkBoxWidth
            indicator.layer.textureSize.height: checkBoxWidth

            text: ""

            nextCheckState: function() {
                    var result
                    var result_bool

                    if (checkState === Qt.Checked){
                        result = Qt.Unchecked
                        result_bool = false
                    } else {
                        result = Qt.Checked
                        result_bool = true
                    }
                    audioCodecStateChange(audioCodecId,result_bool)
                    return result
                }
        }

        Label{
            id: formatNameLabel

            Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            horizontalAlignment: Qt.AlignLeft
            verticalAlignment: Qt.AlignVCenter

            Layout.fillWidth: true
            Layout.fillHeight: true

            text: audioCodecName + " " + samplerRate + " Hz"
            font.pointSize: 8
            font.kerning: true
        }
    }
}
