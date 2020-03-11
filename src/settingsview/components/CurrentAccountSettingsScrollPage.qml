import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

Rectangle {
    anchors.fill: parent

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
            Layout.maximumHeight: 30
            Layout.minimumHeight: 0
            Layout.preferredHeight: accountPageTitle.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            Label {
                id: accountPageTitle

                Layout.preferredWidth: 117

                Layout.maximumHeight: 25
                Layout.preferredHeight: 25
                Layout.minimumHeight: 25

                text: qsTr("Jami Account")

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
        ScrollView {
            id: scrowView

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
