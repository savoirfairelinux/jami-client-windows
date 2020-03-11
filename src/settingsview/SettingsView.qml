import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

import "components"

Window {
    id: settingsViewWindow

    property int minWidth: 768
    property int minHeight: 768
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: 768
    height: 768
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: settingsViewRect
        anchors.fill: parent

        CT.SplitView {
            anchors.fill: parent
            orientation: Qt.Horizontal

            Rectangle {
                id: leftSettingsWidget

                Layout.minimumWidth: 380
                Layout.preferredWidth: 380
                Layout.maximumWidth: parent.width / 2
                Layout.fillHeight: true
                LeftPanelView {
                    id: leftPanelView
                }
            }

            Rectangle {
                id: rightSettingsWidget

                Layout.fillWidth: true
                Layout.fillHeight: true

                color: "red"
            }
        }
    }
}
