import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12

Window {
    id: callStackViewWindow

    property int minWidth: 300
    property int minHeight: 400
    property int textFontSize: 9

    title: "Incoming Call"
    visible: true

    minimumWidth: minWidth
    minimumHeight: minHeight

    maximumWidth: minWidth + 100
    maximumHeight: minHeight + 100

    //flags: Qt.SubWindow | Qt.Tool | Qt.FramelessWindowHint | Qt.WindowSystemMenuHint | Qt.WindowStaysOnTopHint

    AudioCallPage {
        id: audioCallPage
    }

    IncomingCallPage {
        id: incomingCallPage

        onCallAnswerButtonIsClicked: {
            callStackMainView.push(audioCallPage)
        }
    }

    StackView {
        id: callStackMainView

        anchors.fill: parent

        initialItem: incomingCallPage
    }
}
