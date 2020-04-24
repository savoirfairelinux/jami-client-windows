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
}
