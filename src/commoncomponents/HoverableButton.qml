import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0

Button {
    id: hoverableButton

    property int fontPointSize: 9
    property int buttonImageHeight: hoverableButtonBackground.height - 10
    property int buttonImageWidth: hoverableButtonBackground.width - 10
    property string backgroundColor: JamiTheme.releaseColor
    property string onPressColor: JamiTheme.pressColor
    property string onReleaseColor: JamiTheme.releaseColor
    property string onEnterColor: JamiTheme.hoverColor
    property string onExitColor: JamiTheme.releaseColor

    property alias radius: hoverableButtonBackground.radius
    property alias source: hoverableButtonImage.source

    font.pointSize: fontPointSize
    font.kerning: true

    hoverEnabled: true

    background: Rectangle {
        id: hoverableButtonBackground

        color: backgroundColor

        Image {
            id: hoverableButtonImage

            anchors.centerIn: hoverableButtonBackground

            height: buttonImageHeight
            width: buttonImageWidth

            fillMode: Image.PreserveAspectFit
            mipmap: true
            asynchronous: true
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onPressed: {
                hoverableButtonBackground.color = onPressColor
            }
            onReleased: {
                hoverableButtonBackground.color = onReleaseColor
                hoverableButton.clicked()
            }
            onEntered: {
                hoverableButtonBackground.color = onEnterColor
            }
            onExited: {
                hoverableButtonBackground.color = onExitColor
            }
        }
    }
}
