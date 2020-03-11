import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14

import "../../constant"

Button {
    id: button
    checkable: true
    hoverEnabled: true

    property alias imageSource: buttonPix.source
    property alias buttonText: buttonText.text

    property string backgroundColor: JamiTheme.releaseColor
    property string onPressColor: JamiTheme.pressColor
    property string onReleaseColor: JamiTheme.releaseColor
    property string onEnterColor: JamiTheme.hoverColor
    property string onExitColor: JamiTheme.transparentColor
    property string checkedColor: JamiTheme.releaseColor

    Layout.minimumHeight: 60
    Layout.preferredHeight: 60
    Layout.maximumHeight: 60

    Layout.fillWidth: true

    background: Rectangle {
        anchors.fill: parent
        color: parent.checked ? button.checkedColor : button.onExitColor

        RowLayout {
            anchors.fill: parent
            spacing: 24
            Image {
                id: buttonPix
                Layout.minimumHeight: parent.height
                Layout.preferredHeight: parent.height
                Layout.maximumHeight: parent.height

                Layout.minimumWidth: parent.height
                Layout.preferredWidth: parent.height
                Layout.maximumWidth: parent.height

                Layout.leftMargin: 24

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }

            Label {
                id: buttonText

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                Layout.fillHeight: true
                Layout.fillWidth: true

                font.pointSize: 11
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            anchors.fill: parent

            hoverEnabled: true

            onPressed: {
                parent.color = button.onPressColor
            }
            onReleased: {
                parent.color = button.onReleaseColor
                button.clicked()
            }
            onEntered: {
                parent.color = button.onEnterColor
            }
            onExited: {
                button.checked ? parent.color = button.checkedColor : parent.color
                                 = button.onExitColor
            }
        }
    }
}
