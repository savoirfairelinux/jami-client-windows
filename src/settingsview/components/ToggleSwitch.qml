import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

RowLayout {
    property string labelText: value
    property int widthOfSwitch: 50
    property int heightOfSwitch: 10
    property int widthOfLayout: 30
    property int fontPointSize: 13

    property alias toggleSwitch: switchOfLayout

    spacing: 18
    Layout.fillWidth: true
    Layout.maximumHeight: 30

    Switch {
        id: switchOfLayout
        Layout.alignment: Qt.AlignVCenter

        Layout.maximumWidth: widthOfSwitch
        Layout.preferredWidth: widthOfSwitch
        Layout.minimumWidth: widthOfSwitch

        Layout.minimumHeight: heightOfSwitch
        Layout.preferredHeight: heightOfSwitch
        Layout.maximumHeight: heightOfSwitch
    }

    Label {
        Layout.fillWidth: true

        Layout.minimumHeight: widthOfLayout
        Layout.preferredHeight: widthOfLayout
        Layout.maximumHeight: widthOfLayout

        text: qsTr(labelText)
        font.pointSize: fontPointSize
        font.kerning: true

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
    }
}
