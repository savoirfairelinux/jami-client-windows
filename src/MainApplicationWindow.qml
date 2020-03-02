import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14

ApplicationWindow {
    id: mainApplicationWindow

    property int minWidth: 500
    property int minHeight: 500
    property int textPointSize: 8

    Universal.theme: Universal.Light

    title: "Jami"
    visible: true
    width: 600
    minimumWidth: minWidth
    minimumHeight: minHeight

    Loader {
        id: mainViewLoader
    }

    MouseArea {
        anchors.fill: parent
        //onClicked: mainViewLoader.source = "mainview/MainView.qml"
        onClicked: mainViewLoader.source = "WizardView.qml"
    }

    overlay.modal: ColorOverlay {
        source: mainApplicationWindow.contentItem
        color: "transparent"
        // color animation
        ColorAnimation on color {
            to: Qt.rgba(0, 0, 0, 0.33)
            duration: 500
        }
    }


    /*onScreenChanged: {
        mainApplicationWindow.displayScreenChanged()
    }*/
}
