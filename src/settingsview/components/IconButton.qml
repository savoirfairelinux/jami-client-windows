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

import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14

import "../../constant"

Button {
    id: button
    checkable: true
    hoverEnabled: true

    property alias imageSource: buttonPix.source
    property alias buttonText: buttonText.text

    property string backgroundColor: JamiTheme.releaseColor
    property string onPressColor: JamiTheme.pressColor
    property string onReleaseColor: JamiTheme.releaseColor
    property string onEnterColor: JamiTheme.hoverColor
    property string onExitColor: JamiTheme.transparentColor
    property string checkedColor: JamiTheme.releaseColor

    signal checkedToggledForLeftPanel(var checked)
    signal checkedToggledForRightPanel(var checked)

    function setCheckedState(check, triggerSignal) {
        button.checked = check
        if (triggerSignal) {
            checkedToggledForLeftPanel(check)
            checkedToggledForRightPanel(check)
        }
        button.background.color = check ? button.checkedColor : button.onExitColor
    }

    onClicked: {
        setCheckedState(true, true)
    }

    Layout.minimumHeight: 60
    Layout.preferredHeight: 60
    Layout.maximumHeight: 60

    Layout.fillWidth: true

    background: Rectangle {
        anchors.fill: parent
        color: parent.checked ? button.checkedColor : button.onExitColor

        RowLayout {
            anchors.fill: parent
            spacing: 24
            Image {
                id: buttonPix
                Layout.minimumHeight: 24
                Layout.preferredHeight: 24
                Layout.maximumHeight: 24

                Layout.minimumWidth: 24
                Layout.preferredWidth: 24
                Layout.maximumWidth: 24

                Layout.leftMargin: 24

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Label {
                id: buttonText

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                Layout.fillHeight: true
                Layout.fillWidth: true

                font.pointSize: 11
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onPressed: {
                if (!button.checked) {
                    parent.color = button.onPressColor
                }
            }
            onReleased: {
                button.clicked()
                if (!button.checked) {
                    parent.color = button.onExitColor
                }
            }
            onEntered: {
                if (!button.checked) {
                    parent.color = button.onEnterColor
                }
            }
            onExited: {
                if (!button.checked) {
                    parent.color = button.onExitColor
                }
            }
        }
    }
}
