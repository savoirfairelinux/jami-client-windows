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

    property string acceptButtonGreen: "#4caf50"
    property string acceptButtonHoverGreen: "#5db761"
    property string acceptButtonPressedGreen: "#449d48"

    property string declineButtonRed: "#f44336"
    property string declineButtonHoverRed: "#f5554a"
    property string declineButtonPressedRed: "#db3c30"

    property string hangUpButtonTintedRed: "#ff0000"
    property string buttonTintedBlue: "#00aaff"

    // font
    property string faddedFontColor: "#c0c0c0"
    property string faddedLastInteractionFontColor: "#868686"

    property int textFontSize: 9


    // Jami theme colors
    property string blue_ :"#109ede"
    property string lightBlue_: "#c1ebf0"
    property string lightGrey_: Qt.rgba(242, 242, 242,1)
    property string imGrey_: "#dedee0"
    property string imBlue_: "#cfebf5"
    property string lightBlack_: Qt.rgba(63, 63, 63,1)
    property string grey_: Qt.rgba(160, 160, 160 ,1)
    property string red_: Qt.rgba(251, 72, 71,1)
    property string lightRed_: Qt.rgba(252, 91, 90,1)
    property string darkRed_: Qt.rgba(219, 55, 54,1)
    property string notificationRed_:Qt.rgba( 255, 59, 48 ,1)
    property string urgentOrange_:Qt.rgba( 255, 165, 0 ,1)
    property string green_: Qt.rgba(127, 255, 0,1)
    property string presenceGreen_:Qt.rgba( 76, 217, 100 ,1)
    property string smartlistSelection_: Qt.rgba( 240, 240, 240 ,1)
    property string smartlistHighlight_: Qt.rgba( 245, 245, 245 ,1)

        //avatar colors
    property string defaultAvatarColor_ :"#ff9e9e9e" // Gray
    property Item avatarColors_: Item{
            property string avatarRed : "#fff44336" //Red
            property string avatarPink : "#ffe91e63" //Pink
            property string avatarPurple : "#ff9c27b0" //Purple
            property string avatarDeepPurple : "#ff673ab7" //Deep Purple
            property string avatarIndigo : "#ff3f51b5" //Indigo
            property string avatarBlue : "#ff2196f3" //Blue
            property string avatarCyan : "#ff00bcd4" //Cyan
            property string avatarTeal : "#ff009688" //Teal
            property string avatarGreen : "#ff4caf50" //Green
            property string avatarLightGreen : "#ff8bc34a" //Light Green
            property string avatarGrey : "#ff9e9e9e" //Grey
            property string avatarLime : "#ffcddc39" //Lime
            property string avatarAmber : "#ffffc107" //Amber
            property string avatarDeepOrange : "#ffff5722" //Deep Orange
            property string avatarBrown : "#ff795548" //Brown
            property string avatarBlueGrey : "#ff607d8b"  //Blue Grey
    }
}
