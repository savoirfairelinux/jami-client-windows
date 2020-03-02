import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 6

    property bool successState: true
    property string progressLabelEditText : "Generating your Jami account"

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

        state: successState ? "spinnerLabel_Regular" : "spinnerLabel_Failure"
        states:[
            State {
                name: "spinnerLabel_Regular"
                PropertyChanges {
                    target: spinnerLabel

                    background: AnimatedImage {
                        source: "qrc:/images/jami_eclipse_spinner.gif"

                        playing: true
                        paused: false
                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                    }
                }
            },
            State {
                name: "spinnerLabel_Failure"
                PropertyChanges {
                    target: spinnerLabel

                    background: Image{
                        anchors.fill: parent.target
                        Image {
                            anchors.fill: parent
                            source:"image://tintedPixmap/" + ("qrc::/images/icons/baseline-error_outline-24px.svg").replace("qrc:/images/icons/", "") + "+" + JamiTheme.urgentOrange_
                        }
                    }
                }
            }
        ]
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
        text: successState? progressLabelEditText: "Error creating account"
        font.pointSize: 11
        font.kerning: true

        state: successState? "color_success" : "color_fail"
        states: [
            State {
                name: "color_success"
                PropertyChanges {
                    target: progressLabel
                    color: "transparent"
                }
            },
            State {
                name: "color_fail"
                PropertyChanges {
                    target: progressLabel
                    color: "red"
                }
            }
        ]
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
