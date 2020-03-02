pragma Singleton

import QtQuick 2.14

//https://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType-3
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

    // font
    property string faddedFontColor: "#c0c0c0"
    property string faddedLastInteractionFontColor: "#505050"

    property int splitViewHandlePreferedWidth: 4
    property int textFontSize: 9
    // place holder text
    property string contactSearchBarPlaceHolderConversationText: qsTr("Find a new or existing contact")
    property string contactSearchBarPlaceHolderInivitionText: qsTr("Search your received invitations")

    // Jami theme colors
    function rgb256(r, g, b) {
        return Qt.rgba(r / 256, g / 256, b / 256, 1)
    }

    property color blue_: "#109ede"
    property color lightBlue_: "#c1ebf0"
    property color lightGrey_: rgb256(242, 242, 242)
    property color imGrey_: "#dedee0"
    property color imBlue_: "#cfebf5"
    property color lightBlack_: rgb256(63, 63, 63)
    property color grey_: rgb256(160, 160, 160)
    property color red_: rgb256(251, 72, 71)
    property color lightRed_: rgb256(252, 91, 90)
    property color darkRed_: rgb256(219, 55, 54)
    property color notificationRed_: rgb256(255, 59, 48)
    property color urgentOrange_: rgb256(255, 165, 0)
    property color green_: rgb256(127, 255, 0)
    property color presenceGreen_: rgb256(76, 217, 100)
    property color smartlistSelection_: rgb256(240, 240, 240)
    property color smartlistHighlight_: rgb256(245, 245, 245)

    //avatar colors
    property color defaultAvatarColor_: "#ff9e9e9e" // Gray
    property Item avatarColors_: Item {
        property color avatarRed: "#fff44336" //Red
        property color avatarPink: "#ffe91e63" //Pink
        property color avatarPurple: "#ff9c27b0" //Purple
        property color avatarDeepPurple: "#ff673ab7" //Deep Purple
        property color avatarIndigo: "#ff3f51b5" //Indigo
        property color avatarBlue: "#ff2196f3" //Blue
        property color avatarCyan: "#ff00bcd4" //Cyan
        property color avatarTeal: "#ff009688" //Teal
        property color avatarGreen: "#ff4caf50" //Green
        property color avatarLightGreen: "#ff8bc34a" //Light Green
        property color avatarGrey: "#ff9e9e9e" //Grey
        property color avatarLime: "#ffcddc39" //Lime
        property color avatarAmber: "#ffffc107" //Amber
        property color avatarDeepOrange: "#ffff5722" //Deep Orange
        property color avatarBrown: "#ff795548" //Brown
        property color avatarBlueGrey: "#ff607d8b" //Blue Grey
    }
}
