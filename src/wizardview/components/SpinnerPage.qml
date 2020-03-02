import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 6

    Item {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: 40
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Label {
        id: spinnerLabel
        Layout.alignment: Qt.AlignHCenter
        Layout.minimumWidth: 200
        Layout.minimumHeight: 200

        Layout.maximumWidth: 16777215
        Layout.maximumHeight: 16777215

        background: AnimatedImage {
            playing: true
            paused: false
            source: "qrc:/images/jami_eclipse_spinner.gif"
            fillMode: Image.PreserveAspectFit
            mipmap: true
        }
    }
    Item {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: 40
        Layout.fillWidth: true
        Layout.fillHeight: true
    }
    Label {
        id: progressLabel
        Layout.alignment: Qt.AlignHCenter
        text: "Generating your Jami account"
        font.pointSize: 11
        font.kerning: true
    }
    Item {
        Layout.alignment: Qt.AlignHCenter
        Layout.minimumHeight: 20
        Layout.maximumHeight: 20
        Layout.preferredHeight: 20
        Layout.fillWidth: true
        Layout.fillHeight: false
    }
}
