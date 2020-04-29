import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0

Button {
    id: tintedButton

    property string tintColor: "white"
    // in image provider format
    property string normalPixmapSource: ""
    property string selectedPixmapSource: ""

    property bool isSelected: false

    signal buttonEntered

    function setChecked(checked) {
        isSelected = checked
        if (isSelected) {
            tintedButtonImage.source = selectedPixmapSource
        } else {
            tintedButtonImage.source = normalPixmapSource
        }
    }

    hoverEnabled: true

    background: Rectangle {
        id: tintedButtonBackground

        radius: 30
        color: "transparent"

        Image {
            id: tintedButtonImage

            anchors.centerIn: tintedButtonBackground

            height: tintedButtonBackground.height - 10
            width: tintedButtonBackground.width - 10

            source: normalPixmapSource
            fillMode: Image.PreserveAspectFit
            mipmap: true
            asynchronous: true
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onReleased: {
                isSelected = !isSelected
                if (isSelected) {
                    tintedButtonImage.source = "image://tintedPixmap/"
                            + selectedPixmapSource.replace(
                                "qrc:/images/icons/", "") + "+" + tintColor
                } else {
                    tintedButtonImage.source = "image://tintedPixmap/" + normalPixmapSource.replace(
                                "qrc:/images/icons/", "") + "+" + tintColor
                }
                tintedButton.clicked()
            }
            onEntered: {
                // tinted
                if (isSelected) {
                    tintedButtonImage.source = "image://tintedPixmap/"
                            + selectedPixmapSource.replace(
                                "qrc:/images/icons/", "") + "+" + tintColor
                } else {
                    tintedButtonImage.source = "image://tintedPixmap/" + normalPixmapSource.replace(
                                "qrc:/images/icons/", "") + "+" + tintColor
                }
                tintedButton.buttonEntered()
            }
            onExited: {
                if (isSelected) {
                    tintedButtonImage.source = selectedPixmapSource
                } else {
                    tintedButtonImage.source = normalPixmapSource
                }
            }
        }
    }
}
