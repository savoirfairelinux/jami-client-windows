import QtQuick 2.0
import QtQuick.Layouts 1.3

Item {
    GridLayout {
        id: containerWidget
        anchors.fill: parent
        Layout.fillHeight: true
        Layout.fillWidth: true
        RowLayout {
            anchors.fill: parent
            Layout.fillHeight: true
            Layout.fillWidth: true
            ColumnLayout {
                Layout.alignment: Qt.AlignCenter

                Layout.maximumWidth: parent.width * (2 / 3)
                Layout.maximumWidth: parent.height * (2 / 3)
                Layout.minimumWidth: parent.width * (1 / 3)
                Layout.minimumHeight: parent.height * (1 / 3)
                Layout.preferredWidth: 512
                Layout.preferredHeight: 638

                topMargin: 0
                bottomMargin: 30
                leftMargin: 30
                rightMargin: 30

                spacing: 0

                RowLayout {
                    topMargin: 10
                    spacing: 6

                    StackLayout {// welcome page

                        // create account page

                        // create SIP account page

                        // import form backup page

                        // backup keys page

                        // import form device page

                        // connect to account manage page

                        // spinner page
                    }
                }
            }
        }
    }
}
