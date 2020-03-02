import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

import "../../constant"

ColumnLayout {
    Layout.fillWidth: true
    Layout.fillHeight: true
    spacing: 6

    property bool successState: true
    property string progressLabelEditText: "Generating your Jami account"

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

        property string spinnerDisplyState: successState ? "spinnerLabel_Regular" : "spinnerLabel_Failure"
        onSpinnerDisplyStateChanged: {
            switch (spinnerDisplyState) {
            case "spinnerLabel_Regular":
                background = Qt.createQmlObject("import QtQuick 2.14;
AnimatedImage {
source: \"qrc:/images/jami_eclipse_spinner.gif\"

playing: true
paused: false
fillMode: Image.PreserveAspectFit
mipmap: true
}", spinnerLabel)
                break
            case "spinnerLabel_Failure":
                background = Qt.createQmlObject("import QtQuick 2.14;
import \"qrc:/src/constant/\";
Image {
anchors.fill: parent;
source:\"image://tintedPixmap/\" + (\"qrc:/images/icons/baseline-error_outline-24px.svg\").replace(\"qrc:/images/icons/\", \"\") + \"+\" + JamiTheme.urgentOrange_;
}", spinnerLabel)
                break
            }
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
        text: successState ? progressLabelEditText : "Error creating account"
        font.pointSize: 11
        font.kerning: true

        property string progressLabelState: successState ? "color_success" : "color_fail"
        onProgressLabelStateChanged: {
            switch (progressLabelState) {
            case "color_success":
                background = Qt.createQmlObject(
                            "import QtQuick 2.14; Rectangle { anchors.fill: parent; color: \"transparent\"; }",
                            progressLabel)
                break
            case "color_fail":
                background = Qt.createQmlObject(
                            "import QtQuick 2.14; Rectangle { anchors.fill: parent; color: \"red\"; }",
                            progressLabel)
                break
            }
        }
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
