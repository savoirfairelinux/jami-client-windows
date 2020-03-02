import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.jamitheme 1.0

Button {
    id: hoverableButton

    property int fontPointSize: 9
    property string backgroundColor: JamiTheme.releaseColor
    property string onPressColor: JamiTheme.pressColor
    property string onReleaseColor: JamiTheme.releaseColor
    property string onEnterColor: JamiTheme.hoverColor
    property string onExitColor: JamiTheme.releaseColor

    property alias radius: aboutButtonBackground.radius
    property alias source: hoverableButtonImage.source

    font.pointSize: fontPointSize

    hoverEnabled: true

    background: Rectangle {
        id: aboutButtonBackground

        color: backgroundColor

        Image {
            id: hoverableButtonImage

            anchors.centerIn: aboutButtonBackground

            height: aboutButtonBackground.height - 10
            width: aboutButtonBackground.width - 10

            fillMode: Image.PreserveAspectFit
            mipmap: true
            asynchronous: true
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onPressed: { aboutButtonBackground.color = onPressColor; }
            onReleased: {
                aboutButtonBackground.color = onReleaseColor
                hoverableButton.clicked()
            }
            onEntered: { aboutButtonBackground.color = onEnterColor; }
            onExited: { aboutButtonBackground.color = onExitColor; }
        }
    }
}
