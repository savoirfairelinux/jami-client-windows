import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

GridLayout {
    Layout.fillWidth: true
    rowSpacing: 6
    columnSpacing: 6

    rows: 2
    columns: 2

    TextField {
        id: passwordEdit

        Layout.row: 0
        Layout.column: 0

        visible: choosePasswordCheckBox.checked
                 && choosePasswordCheckBox.visible

        Layout.maximumWidth: 261
        Layout.preferredWidth: 261

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 25

        Layout.alignment: Qt.AlignHCenter

        placeholderText: qsTr("Passowrd")
        font.pointSize: 10
        font.kerning: true
    }

    Item {
        Layout.row: 0
        Layout.column: 1

        Layout.maximumWidth: 32
        Layout.preferredWidth: 32
        Layout.minimumWidth: 32

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30
    }

    TextField {
        id: confirmPasswordEdit

        Layout.row: 0
        Layout.column: 1

        visible: choosePasswordCheckBox.checked
                 && choosePasswordCheckBox.visible

        Layout.maximumWidth: 261
        Layout.preferredWidth: 261

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 25

        Layout.alignment: Qt.AlignHCenter

        placeholderText: qsTr("Confirm Passowrd")
        font.pointSize: 10
        font.kerning: true
    }

    Label {
        id: passwordStatusLabel

        Layout.maximumWidth: 32
        Layout.preferredWidth: 32
        Layout.minimumWidth: 32

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        Layout.alignment: Qt.AlignRight

        state: "Blank"

        states: [
            State {
                name: "Hide"
                PropertyChanges {
                    target: lookupStatusLabel
                    background: Rectangle {
                        anchors.fill: parent.target
                        color: "transparent"
                    }
                }
            },
            State {
                name: "Fail"
                PropertyChanges {
                    target: lookupStatusLabel
                    background: Rectangle {
                        anchors.fill: parent.target
                        Image {
                            anchors.fill: parent
                            source: "image://tintedPixmap/"
                                    + ("qrc::/images/icons/baseline-close-24px.svg").replace(
                                        "qrc:/images/icons/",
                                        "") + "+" + JamiTheme.red_
                        }
                    }
                }
            },
            State {
                name: "Success"
                PropertyChanges {
                    target: lookupStatusLabel
                    background: Rectangle {
                        anchors.fill: parent.target
                        Image {
                            anchors.fill: parent
                            source: "image://tintedPixmap/"
                                    + ("qrc::/images/icons/baseline-done-24px.svg").replace(
                                        "qrc:/images/icons/",
                                        "") + "+" + JamiTheme.presenceGreen_
                        }
                    }
                }
            }
        ]
    }
}
