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

    // font color
    property string faddedFontColor: "#c0c0c0"
    property string faddedLastInteractionFontColor: "#868686"
}
