import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

Button {
    id: hoverableButton

    property alias radius: aboutButtonBackground.radius
    property int fontPointSize: 9
    property alias source: hoverableButtonImage.source

    font.pointSize: fontPointSize

    hoverEnabled: true

    background: Rectangle {
        id: aboutButtonBackground

        color: ColorConstant.releaseColor

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

            onPressed: { aboutButtonBackground.color = ColorConstant.pressColor; }
            onReleased: {
                aboutButtonBackground.color = ColorConstant.releaseColor
                hoverableButton.clicked()
            }
            onEntered: { aboutButtonBackground.color = ColorConstant.hoverColor; }
            onExited: { aboutButtonBackground.color = ColorConstant.releaseColor; }
        }
    }
}
