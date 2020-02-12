import QtQuick 2.14
import QtQml 2.14
import QtQuick.Window 2.13
import D3D11QML 1.0

//Window {
//   width: 320
//    height: 480
//    id: base
//    visible: true
//    flags: Qt.Window

//opacity: 1.0
//color: "red"
Window {
    id: distantWindow
    visible: true
    opacity: 1.0
    width: 480
    height: 270
    //x: base.x
    //y: base.y
    flags: Qt.Window

    D3D11QtQuickVideoDistantWidget {
        id: distant
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onClicked: {
                label.text = "Mouse is on the distant, position: " + parent.x.toString(
                            ) + ", " + parent.y.toString()
            }
        }
    }

    Window {
        id: previewWindow
        visible: true

        width: distantWindow.width / 5
        height: distantWindow.height / 5
        x: distantWindow.x + distantWindow.width - (width + 20)
        y: distantWindow.y + distantWindow.height - (height + 20)

        flags: Qt.Window | Qt.FramelessWindowHint

        D3D11QtQuickVideoPreviewWidget {
            id: preview
            anchors.fill: parent

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    label.text = "Mouse is on the preview, position: " + parent.x.toString(
                                ) + ", " + parent.y.toString()
                }
            }
        }
    }
} //Rectangle {//    color: Qt.rgba(1, 0, 0, 0.7)//   radius: 10//   border.width: 1//    border.color: "white"//    anchors.fill: label
//    anchors.margins: -10
//}

//Text {
//    id: label
//    color: "black"
//    wrapMode: Text.WordWrap
//    text: "The background here is a squircle rendered with raw Direct3D 11 using the beforeRendering() and beforeRenderPassRecording() signals in QQuickWindow. This text label and its border is rendered using QML"
//    anchors.right: parent.right
//   anchors.left: parent.left
//     anchors.bottom: parent.bottom
//     anchors.margins: 20
//}
//}

