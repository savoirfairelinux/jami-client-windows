import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Rectangle {
    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 10
            Layout.minimumHeight: 10
            Layout.preferredHeight: 10

            Layout.alignment: Qt.AlignTop
        }

        RowLayout {
            spacing: 6

            Layout.alignment: Qt.AlignTop

            Layout.fillWidth: true
            Layout.maximumHeight: 31
            Layout.minimumHeight: 0
            Layout.preferredHeight: accountPageTitleSIP.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 48
                Layout.preferredWidth: 48
                Layout.minimumWidth: 48
            }

            Label {
                id: accountPageTitleSIP

                Layout.preferredWidth: 133

                Layout.preferredHeight: 31
                Layout.minimumHeight: 25

                text: qsTr("SIP Account")

                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
