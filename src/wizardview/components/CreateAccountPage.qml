import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import net.jami.NewWizardViewQmlObjectHolder 1.0

import "../../constant"

ColumnLayout {
    property var nameRegistrationUIState: NewWizardViewQmlObjectHolder.BLANK

    property alias fullNameEditAlias: fullNameEdit
    property alias usernameEditAlias: usernameEdit
    property alias passwordEditAlias: collapsiblePasswordWidget.passwordEditAlias
    property alias confirmPasswordEditAlias: collapsiblePasswordWidget.confirmPasswordEditAlias

    property alias lookupStatusLabelAlias: lookupStatusLabel
    property alias signUpCheckboxAlias: signUpCheckbox
    property alias passwordStatusLabelAlias: collapsiblePasswordWidget.passwordStatusLabelAlias

    property alias choosePasswordCheckBoxAlias: choosePasswordCheckBox
    property alias collapsiblePasswordWidgetAlias: collapsiblePasswordWidget

    function initializeOnShowUp() {
        usernameEdit.clear()
        passwordEdit.clear()
        confirmPasswordEdit.clear()

        signUpCheckbox.checked = true
        choosePasswordCheckBox.checked = false
        usernameEdit.enabled = true
        fullNameEdit.enabled = true
        fullNameEdit.text = ""
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

                placeholderText: qsTr("Choose your username")
                font.pointSize: 10
                font.kerning: true
            }

            Label {
                id: lookupStatusLabel

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30

                Layout.maximumHeight: 30
                Layout.preferredHeight: 30
                Layout.minimumHeight: 30

                state: {
                    switch (nameRegistrationUIState) {
                    case NewWizardViewQmlObjectHolder.BLANK:
                        return "Blank"
                    case NewWizardViewQmlObjectHolder.INVALID:
                        return "Invalid"
                    case NewWizardViewQmlObjectHolder.TAKEN:
                        return "Taken"
                    case NewWizardViewQmlObjectHolder.FREE:
                        return "Free"
                    case NewWizardViewQmlObjectHolder.SEARCHING:
                        return "Searching"
                    }
                }

                states: [
                    State {
                        name: "Blank"
                        PropertyChanges {
                            target: lookupStatusLabel
                            background: Rectangle {
                                anchors.fill: parent.target
                                color: "transparent"
                            }
                        }
                    },
                    State {
                        name: "Invalid"
                        PropertyChanges {
                            target: lookupStatusLabel
                            background: Rectangle {
                                anchors.fill: parent.target
                                Image {
                                    anchors.fill: parent
                                    source: "image://tintedPixmap/" + ("qrc:/images/icons/baseline-error_outline-24px.svg").replace(
                                                "qrc:/images/icons/",
                                                "") + "+" + JamiTheme.urgentOrange_
                                }
                            }
                        }
                    },
                    State {
                        name: "Taken"
                        PropertyChanges {
                            target: lookupStatusLabel
                            background: Rectangle {
                                anchors.fill: parent.target
                                Image {
                                    anchors.fill: parent
                                    source: "image://tintedPixmap/" + ("qrc::/images/icons/baseline-close-24px.svg").replace(
                                                "qrc:/images/icons/",
                                                "") + "+" + JamiTheme.red_
                                }
                            }
                        }
                    },
                    State {
                        name: "Free"
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
                    },
                    State {
                        name: "Searching"
                        PropertyChanges {
                            target: lookupStatusLabel
                            background: Rectangle {
                                anchors.fill: parent.target

                                AnimatedImage {
                                    anchors.fill: parent
                                    source: "qrc:/images/jami_rolling_spinner.gif"

                                    playing: true
                                    paused: false
                                    fillMode: Image.PreserveAspectFit
                                    mipmap: true
                                }
                            }
                        }
                    }
                ]
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

                text: qsTr("Register public username")
                font.pointSize: 8
                font.kerning: true
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
