import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14

GridLayout {
    id: root

    property alias text_passwordEditAlias: passwordEdit.text
    property alias text_confirmPasswordEditAlias: confirmPasswordEdit.text
    property alias state_passwordStatusLabelAlias: passwordStatusLabel.passwordStatusState

    property bool visibleCollapsble: false

    function clearAllTextFields() {
        passwordEdit.clear()
        confirmPasswordEdit.clear()
    }

    visible: visibleCollapsble
    Layout.fillWidth: true
    rowSpacing: 6
    columnSpacing: 6

    rows: 2
    columns: 2

    TextField {
        id: passwordEdit

        visible: visibleCollapsble

        Layout.row: 0
        Layout.column: 0

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

        visible: visibleCollapsble

        Layout.row: 0
        Layout.column: 1

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

        visible: visibleCollapsble

        Layout.maximumWidth: 32
        Layout.preferredWidth: 32
        Layout.minimumWidth: 32

        Layout.maximumHeight: 30
        Layout.preferredHeight: 30
        Layout.minimumHeight: 30

        Layout.alignment: Qt.AlignRight

        property string passwordStatusState: "Hide"

        background: {
            switch (passwordStatusState) {
            case "Hide":
                return Qt.createQmlObject(
                            "import QtQuick 2.14; Rectangle { anchors.fill: parent; color: \"transparent\"; }",
                            passwordStatusLabel)
            case "Fail":
                return Qt.createQmlObject(
                            "import QtQuick 2.14; Rectangle { anchors.fill: parent; Image{anchors.fill: parent; source: \"image://tintedPixmap/\"+ (\"qrc::/images/icons/baseline-close-24px.svg\").replace(\"qrc:/images/icons/\",\"\") + \"+\" + JamiTheme.red_;}; }",
                            passwordStatusLabel)
            case "Success":
                return Qt.createQmlObject(
                            "import QtQuick 2.14; Rectangle { anchors.fill: parent; Image {
anchors.fill: parent; source: \"image://tintedPixmap/\"+ (\"qrc::/images/icons/baseline-done-24px.svg\").replace(\"qrc:/images/icons/\",\"\") + \"+\" + JamiTheme.presenceGreen_;}; }",
                            passwordStatusLabel)
            }
        }
    }
}
