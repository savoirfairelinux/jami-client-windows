import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import net.jami.NewWizardViewQmlObjectHolder 1.0

import "../"
import "../../constant"

ColumnLayout {
    property alias text_fullNameEditAlias: fullNameEdit.text
    property alias text_usernameEditAlias: usernameEdit.text

    property int nameRegistrationUIState: WizardView.BLANK

    property alias checkState_signUpCheckboxAlias: signUpCheckbox.checked
    property alias isToSetPassword_checkState_choosePasswordCheckBox: choosePasswordCheckBox.checked

    // collapse password widget property aliases
    property alias text_passwordEditAlias: collapsiblePasswordWidget.text_passwordEditAlias
    property alias text_confirmPasswordEditAlias: collapsiblePasswordWidget.text_confirmPasswordEditAlias
    property alias displayState_passwordStatusLabelAlias: collapsiblePasswordWidget.state_passwordStatusLabelAlias

    signal validateWizardProgressionCreateAccountPage

    function initializeOnShowUp() {
        clearAllTextFields()

        signUpCheckbox.checked = true
        choosePasswordCheckBox.checked = false
        usernameEdit.enabled = true
        fullNameEdit.enabled = true
    }

    function clearAllTextFields() {
        usernameEdit.clear()
        fullNameEdit.clear()

        collapsiblePasswordWidget.clearAllTextFields()
    }

    function setCollapsiblePasswordWidgetVisibility(visible) {
        choosePasswordCheckBox.checked = visible
        if (visible) {
            choosePasswordCheckBox.visible = true
        }
    }

    Layout.fillWidth: true
    Layout.fillHeight: true

    spacing: 6

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    Label {
        id: profileSectionLabel

        Layout.maximumWidth: 368
        Layout.preferredWidth: 368
        Layout.maximumHeight: 21
        Layout.preferredHeight: 21

        Layout.alignment: Qt.AlignHCenter

        text: qsTr("Profile")
        font.pointSize: 13
        font.kerning: true

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        id: setAvatarWidget
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: 261
        Layout.preferredWidth: 261
        Layout.minimumWidth: 261
        Layout.maximumHeight: 261
        Layout.preferredHeight: 261
        Layout.minimumHeight: 261

        color: "green"
    }

    RowLayout {
        spacing: 0
        Layout.alignment: Qt.AlignHCenter
        Layout.maximumHeight: 30

        Item {
            Layout.fillWidth: true
            Layout.maximumHeight: 10
        }

        TextField {
            id: fullNameEdit

            Layout.maximumWidth: 261
            Layout.preferredWidth: 261
            Layout.maximumHeight: 30
            Layout.preferredHeight: 30

            Layout.alignment: Qt.AlignCenter

            selectByMouse: true
            placeholderText: qsTr("Profile name")
            font.pointSize: 10
            font.kerning: true
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }

    Item {
        Layout.fillHeight: true
        Layout.fillWidth: true
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignHCenter

        spacing: 5
        Label {
            id: accountSectionLabel
            Layout.alignment: Qt.AlignHCenter

            Layout.maximumWidth: 261
            Layout.preferredWidth: 261
            Layout.minimumWidth: 261
            Layout.maximumHeight: 30
            Layout.preferredHeight: 30
            Layout.minimumHeight: 30

            text: qsTr("Account")
            font.pointSize: 13
            font.kerning: true

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        CheckBox {
            id: signUpCheckbox
            checked: true

            Layout.maximumWidth: 261
            Layout.preferredWidth: 261

            Layout.maximumHeight: 30
            Layout.preferredHeight: 30
            Layout.minimumHeight: 25

            Layout.alignment: Qt.AlignHCenter

            text: qsTr("Register public username")
            font.pointSize: 10
            font.kerning: true

            onClicked: {
                if (!checked) {
                    usernameEdit.clear()
                }

                validateWizardProgressionCreateAccountPage()
            }
        }

        RowLayout {
            spacing: 6
            Layout.fillWidth: true

            TextField {
                id: usernameEdit

                Layout.maximumWidth: 261
                Layout.preferredWidth: 261

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 25

                Layout.alignment: Qt.AlignHCenter

                selectByMouse: true
                placeholderText: qsTr("Choose your username")
                font.pointSize: 10
                font.kerning: true

                enabled: signUpCheckbox.visible && signUpCheckbox.checked
            }

            Label {
                id: lookupStatusLabel

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                visible: true

                property string lookupStatusState: {
                    switch (nameRegistrationUIState) {
                    case WizardView.BLANK:
                        return "Blank"
                    case WizardView.INVALID:
                        return "Invalid"
                    case WizardView.TAKEN:
                        return "Taken"
                    case WizardView.FREE:
                        return "Free"
                    case WizardView.SEARCHING:
                        return "Searching"
                    default:
                        return "Blank"
                    }
                }

                onLookupStatusStateChanged: {
                    switch (lookupStatusState) {
                    case "Blank":
                        background = Qt.createQmlObject(
                                    "import QtQuick 2.14; Rectangle { anchors.fill: parent; color: \"transparent\"; }",
                                    lookupStatusLabel)
                        break
                    case "Invalid":
                        background = Qt.createQmlObject("import QtQuick 2.14;
import \"qrc:/src/constant/\";
Rectangle {
anchors.fill: parent;
Image {
anchors.fill: parent;
source: \"image://tintedPixmap/\" + (\"qrc:/images/icons/baseline-error_outline-24px.svg\").replace(\"qrc:/images/icons/\",\"\") + \"+\" + JamiTheme.urgentOrange_;
mipmap: true;
}
}", lookupStatusLabel)
                        break
                    case "Taken":
                        background = Qt.createQmlObject("import QtQuick 2.14;
import \"qrc:/src/constant/\";
Rectangle {
anchors.fill: parent;
Image{
anchors.fill: parent;
source: \"image://tintedPixmap/\" + (\"qrc:/images/icons/baseline-close-24px.svg\").replace(\"qrc:/images/icons/\",\"\") + \"+\" + JamiTheme.red_;
mipmap: true;
}
}", lookupStatusLabel)
                        break
                    case "Free":
                        background = Qt.createQmlObject("import QtQuick 2.14;
import \"qrc:/src/constant/\";
Rectangle {
anchors.fill: parent;
Image {
anchors.fill: parent;
source: \"image://tintedPixmap/\"+ (\"qrc:/images/icons/baseline-done-24px.svg\").replace(\"qrc:/images/icons/\",\"\") + \"+\" + JamiTheme.presenceGreen_;
mipmap: true;
}
}", lookupStatusLabel)
                        break
                    case "Searching":
                        background = Qt.createQmlObject("import QtQuick 2.14;
import \"qrc:/src/constant/\";
Rectangle {
anchors.fill: parent;
AnimatedImage {
anchors.fill: parent
source: \"qrc:/images/jami_rolling_spinner.gif\";

playing: true
paused: false
fillMode: Image.PreserveAspectFit
mipmap: true
}
}", lookupStatusLabel)
                        break
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6

            CheckBox {
                id: choosePasswordCheckBox
                checked: false

                Layout.maximumWidth: 261
                Layout.preferredWidth: 261

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 25

                Layout.alignment: Qt.AlignHCenter

                text: qsTr("Choose a passowrd for enhanced security")
                font.pointSize: 8
                font.kerning: true

                onClicked: {
                    if (!checked) {
                        collapsiblePasswordWidget.clearAllTextFields()
                    }

                    validateWizardProgressionCreateAccountPage()
                }
            }

            CollapsiblePasswordWidget {
                id: collapsiblePasswordWidget

                visibleCollapsble: choosePasswordCheckBox.checked
                                   && choosePasswordCheckBox.visible
            }
        }

        Item {
            Layout.maximumWidth: 261
            Layout.preferredWidth: 261
            Layout.minimumWidth: 261

            Layout.maximumHeight: 30
            Layout.preferredHeight: 30
            Layout.minimumHeight: 30

            Layout.alignment: Qt.AlignHCenter
        }
    }
}
