import QtQuick 2.0
import QtQuick.Window 2.14
import D3D11QML 1.0

Item {

    width: 320
    height: 480

    //    Rectangle {
    //        anchors.fill: parent
    //        radius: 10
    D3D11QtQuickVideoPreviewWidget {
        id: preview
        width: parent.width
        height: parent.height
    }

    //    }
    Rectangle {
        color: Qt.rgba(1, 0, 0, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "The background here is a squircle rendered with raw Direct3D 11 using the beforeRendering() and beforeRenderPassRecording() signals in QQuickWindow. This text label and its border is rendered using QML"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
} //! [2]
