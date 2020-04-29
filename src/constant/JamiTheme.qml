
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
 * JamiTheme as a singleton is to provide global property entry
 * https://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType-3
 */
pragma Singleton

import QtQuick 2.14

Item {
    // color strings
    property string hoverColor: "#c7c7c7"
    property string pressColor: "#c0c0c0"
    property string releaseColor: "#e0e0e0"
    property string tabbarBorderColor: "#e3e3e3"
    property string transparentColor: "transparent"
    property string presenceGreen: "#4cd964"
    property string notificationRed: "#ff3b30"

    property string acceptButtonGreen: "#4caf50"
    property string acceptButtonHoverGreen: "#5db761"
    property string acceptButtonPressedGreen: "#449d48"

    property string declineButtonRed: "#f44336"
    property string declineButtonHoverRed: "#f5554a"
    property string declineButtonPressedRed: "#db3c30"

    property string hangUpButtonTintedRed: "#ff0000"
    property string buttonTintedBlue: "#00aaff"

    property string selectionBlue: "#109ede"

    property string closeButtonLighterBlack: "#4c4c4c"

    property string contactSearchBarPlaceHolderTextFontColor: "#767676"
    property string contactSearchBarPlaceHolderGreyBackground: "#dddddd"

    property string draftRed: "#cf5300"

    // font
    property string faddedFontColor: "#c0c0c0"
    property string faddedLastInteractionFontColor: "#505050"

    property int splitViewHandlePreferedWidth: 4
    property int textFontSize: 9

    // place holder text
    property string contactSearchBarPlaceHolderConversationText: qsTr("Find a new or existing contact")
    property string contactSearchBarPlaceHolderInivitionText: qsTr("Search your received invitations")
}
