import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import net.jami.AccountModelAdapter 1.0
import net.jami.NewWizardViewQmlObjectHolder 1.0
import net.jami.tools.utils 1.0

import "../constant"
import "components"

Window {
    id: wizardViewWindow

    property int layoutWidth: 401
    property int layoutHeight: 625
    property int textFontSize: 9

    title: "Jami"
    visible: true
    width: layoutWidth
    height: layoutHeight

    Component.onCompleted: {
        newWizardViewQmlObjectHolder.setNewWizardViewQmlObject(wizardViewWindow)
    }

    NewWizardViewQmlObjectHolder {
        id: newWizardViewQmlObjectHolder

        Timer {
            id: failureRedirectPageTimer

            repeat: false
            triggeredOnStart: false
            interval: 1000

            onTriggered: spinnerPage.successState = true
        }

        // reportFailure
        onReportFailure: {
            spinnerPage.successState = false
            failureRedirectPageTimer.restart()
        }

        // updateNameRegistrationUi
        onUpdateNameRegistrationUi: {
            createAccountPage.nameRegistrationUIState = state
        }

        // change page
        onChangePage: {
            changePageQML(pageIndex)
        }

        // validate wizard progression
        onValidateWizardProgression: {
            validateWizardProgressionQML()
        }

        // signal connections for create account page
        Connections{
            target: createAccountPage.usernameEditAlias
            onTextChanged:{

            }
        }

        Connections{
            target: createAccountPage.collapsiblePasswordWidgetAlias.passwordEditAlias

            onTextChanged:{

            }
        }

        Connections{
            target: createAccountPage.collapsiblePasswordWidgetAlias.confirmPasswordEditAlias

            onTextChanged:{

            }
        }

        // function to set up nav bar visibility and the three buttons' visibiliy
        function setNavBarVisibility(navVisible, back) {
            navBarView.visible = navVisible || back
            btnNext.visible = navVisible
            btnPevious.visible = navVisible
            btnBack.visible = back && AccountModelAdapter.getAccountListSize()
        }

        function processWizardInformationsQML() {
            newWizardViewQmlObjectHolder.clearInputPara()

            switch (wizardMode) {
            case NewWizardViewQmlObjectHolder.CREATE:
                spinnerPage.progressLabelEditText = qsTr(
                            "Generating your Jami account...")
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "alias", createAccountPage.fullNameEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "password",
                            createAccountPage.passwordEditAlias.text)
                createAccountPage.fullNameEditAlias.clear()
                createAccountPage.passwordEditAlias.clear()
                createAccountPage.confirmPasswordEditAlias.clear()
                break;
            case NewWizardViewQmlObjectHolder.IMPORT:
                spinnerPage.progressLabelEditText = qsTr(
                            "Importing account archive...")
                // should only work in import from backup page or import from device page
                if (controlPanelStackView.currentIndex
                        == controlPanelStackView.importFromBackupPageId) {
                    newWizardViewQmlObjectHolder.setInputParaHashPair(
                                "password",
                                importFromBackupPage.passwordFromBackupEditAlias.text)
                    importFromBackupPage.passwordFromBackupEditAlias.clear()
                } else if (controlPanelStackView.currentIndex
                           == controlPanelStackView.importFromDevicePageId) {
                    newWizardViewQmlObjectHolder.setInputParaHashPair(
                                "archivePin",
                                importFromDevicePage.pinFromDeviceAlias.text)
                    newWizardViewQmlObjectHolder.setInputParaHashPair(
                                "password",
                                importFromDevicePage.passwordFromDeviceAlias.text)
                    importFromDevicePage.pinFromDeviceAlias.clear()
                    importFromDevicePage.passwordFromDeviceAlias.clear()
                }
                break;
            case NewWizardViewQmlObjectHolder.MIGRATE:
                spinnerPage.progressLabelEditText = qsTr(
                            "Migrating your Jami account...")
                break;
            case NewWizardViewQmlObjectHolder.CREATESIP:
                spinnerPage.progressLabelEditText = qsTr(
                            "Generating your SIP account...")
                if (createSIPAccountPage.sipFullNameEditAlias.text.length == 0) {
                    newWizardViewQmlObjectHolder.setInputParaHashPair("alias",
                                                                      "SIP")
                } else {
                    newWizardViewQmlObjectHolder.setInputParaHashPair(
                                "alias",
                                createSIPAccountPage.sipFullNameEditAlias.text)
                }
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "hostname",
                            createSIPAccountPage.sipServernameEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "username",
                            createSIPAccountPage.sipUsernameEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "password",
                            createSIPAccountPage.sipPasswordEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "proxy",
                            createSIPAccountPage.sipProxyEditAlias.text)
                break;
            case NewWizardViewQmlObjectHolder.CONNECTMANAGER:
                spinnerPage.progressLabelEditText = qsTr(
                            "Connecting to account manager...")
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "username",
                            connectToAccountManagerPage.usernameManagerEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "password",
                            connectToAccountManagerPage.passwordManagerEditAlias.text)
                newWizardViewQmlObjectHolder.setInputParaHashPair(
                            "manager",
                            connectToAccountManagerPage.accountManagerEditAlias.text)

                connectToAccountManagerPage.usernameManagerEditAlias.clear()
                connectToAccountManagerPage.passwordManagerEditAlias.clear()
                connectToAccountManagerPage.accountManagerEditAlias.clear()
                break;
            }

            newWizardViewQmlObjectHolder.setInputParaHashPair(
                        "archivePath",
                        fileToImport)
            if(getIfKeyExistInParaHashPair("archivePin")) {
                newWizardViewQmlObjectHolder.setInputParaHashPair("archivePin","")
            }

            // change page to spinner page
            changePageQML(controlPanelStackView.spinnerPageId)

            //create account
            createAccount()
            UtilsAdapter.createStartupLink("L\"Jami\"")
        }

        function changePageQML(pageIndex){
            if(pageIndex == controlPanelStackView.spinnerPageId) {
                newWizardViewQmlObjectHolder.setNavBarVisibility(false)
            }
            controlPanelStackView.currentIndex = pageIndex
            if(pageIndex == controlPanelStackView.welcomePageStackId) {
                newWizardViewQmlObjectHolder.fileToImport = ""
                newWizardViewQmlObjectHolder.setNavBarVisibility(false,true)
                createAccountPage.lookupStatusLabelAlias.state = "Blank"
            } else if(pageIndex == controlPanelStackView.createAccountPageId){
                createAccountPage.initializeOnShowUp()
                newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                // TODO: connection between register name found and its slot
                // validate wizard progression
                // start photobooth
            } else if(pageIndex == controlPanelStackView.createSIPAccountPageId){
                createSIPAccountPage.initializeOnShowUp()
                newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                btnNext.enabled = true
                // start photo booth
            } else if(pageIndex == controlPanelStackView.importFromDevicePageId) {
                importFromDevicePage.initializeOnShowUp()
                newWizardViewQmlObjectHolder.setNavBarVisibility(true)
            } else if(pageIndex == controlPanelStackView.spinnerPageId){
                createAccountPage.lookupStatusLabelAlias.state = "Blank"
                createAccountPage.choosePasswordCheckBoxAlias.checked = "false"
            }else if(pageIndex == controlPanelStackView.connectToAccountManagerPageId) {
                newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                connectToAccountManagerPage.initializeOnShowUp()
                btnNext.enabled = false
            } else if(pageIndex == controlPanelStackView.importFromBackupPageId) {
                newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                importFromBackupPage.passwordFromBackupEditAlias.clear()
                newWizardViewQmlObjectHolder.fileToImport = ""
                btnNext.enabled = false
            } else if(pageIndex == controlPanelStackView.backupKeysPageId) {
                newWizardViewQmlObjectHolder.setNavBarVisibility(false)
            }
        }

        function validateWizardProgressionQML() {
            if(controlPanelStackView.currentIndex == controlPanelStackView.importFromDevicePageId) {
                var validPin = !(importFromDevicePage.pinFromDeviceAlias.text.length == 0)
                btnNext.enabled = validPin
                return
            } else if(controlPanelStackView.currentIndex == controlPanelStackView.connectToAccountManagerPageId) {
                var validUsername = !(connectToAccountManagerPage.usernameManagerEditAlias.text.length == 0)
                var validPassword = !(connectToAccountManagerPage.passwordManagerEditAlias.text.length == 0)
                var validManager = !(connectToAccountManagerPage.accountManagerEditAlias.text.length == 0)
                btnNext.enabled =validUsername && validPassword && validManager
                return
            } else if(controlPanelStackView.currentIndex == controlPanelStackView.importFromBackupPageId){
                var validImport = !(newWizardViewQmlObjectHolder.fileToImport.length == 0)
                btnNext.enabled = validImport
                return
            }

            var usernameOk = !createAccountPage.signUpCheckboxAlias.checked ||
                             (createAccountPage.signUpCheckboxAlias.checked &&
                              !(newWizardViewQmlObjectHolder.registeredName.length == 0) &&
                              (newWizardViewQmlObjectHolder.registeredName == createAccountPage.usernameEditAlias.text) &&
                              (newWizardViewQmlObjectHolder.registrationStateOk == true))
            var passwordOk =(createAccountPage.passwordEditAlias.text == createAccountPage.confirmPasswordEditAlias.text)

            // set password status label
            if(passwordOk && (createAccountPage.collapsiblePasswordWidgetAlias.passwordEditAlias.text.length == 0)){
                createAccountPage.collapsiblePasswordWidgetAlias.passwordStatusLabelAlias.state = "Success"
            } else if(!passwordOk){
                createAccountPage.collapsiblePasswordWidgetAlias.passwordStatusLabelAlias.state = "Fail"
            } else {
                createAccountPage.collapsiblePasswordWidgetAlias.passwordStatusLabelAlias.state = "Hide"
            }
            //set enable state of next button
            btnNext.enabled = (usernameOk && passwordOk)
        }
    }

    // main frame rectangle
    Rectangle {
        id: wizardViewRect
        anchors.fill: parent

        ColumnLayout {
            id: content
            anchors.fill: parent
            Layout.alignment: Qt.AlignHCenter
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true

                StackLayout {
                    id: controlPanelStackView
                    currentIndex: welcomePageStackId
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    property int welcomePageStackId: 0
                    property int createAccountPageId: 1
                    property int createSIPAccountPageId: 2
                    property int importFromBackupPageId: 3
                    property int backupKeysPageId: 4
                    property int importFromDevicePageId: 5
                    property int connectToAccountManagerPageId: 6
                    property int spinnerPageId: 7

                    WelcomePageLayout {
                        // welcome page, index 0
                        id: welcomePage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            false, true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        false, true)
                        }
                    }

                    CreateAccountPage {
                        // create account page, index 1
                        id: createAccountPage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        true)
                        }
                    }

                    CreateSIPAccountPage {
                        // create SIP account page, index 2
                        id: createSIPAccountPage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        true)
                        }
                    }

                    ImportFromBackupPage {
                        // import from backup page, index 3
                        id: importFromBackupPage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        true)
                        }
                    }

                    BackupKeyPage {
                        // backup keys page, index 4
                        id: backupKeysPage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            false)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        false)
                        }
                    }

                    ImportFromDevicePage {
                        // import from device page, index 5
                        id: importFromDevicePage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        true)
                        }
                    }

                    ConnectToAccountManagerPage {
                        // connect to account manager Page, index 6
                        id: connectToAccountManagerPage

                        onVisibleChanged: {
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            true)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        true)
                        }
                    }

                    SpinnerPage {
                        // spinner Page, index 7
                        id: spinnerPage

                        onVisibleChanged: {
                            // hide all the buttons
                            if (visible)
                                newWizardViewQmlObjectHolder.setNavBarVisibility(
                                            false)
                        }

                        Component.onCompleted: {
                            newWizardViewQmlObjectHolder.setNavBarVisibility(
                                        false)
                        }
                    }
                }
            }

            RowLayout {
                id: navBarView

                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.fillWidth: true

                Layout.maximumHeight: 52
                Layout.preferredHeight: 52

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                RoundButton {
                    id: btnPevious
                    Layout.alignment: Qt.AlignLeft
                    width: 85
                    height: 30
                    radius: height / 2

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Previous")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }

                    onClicked: {
                        // TODO: stop photobooth previewing
                        // Disconnect registered name found Connection
                        // deal with look up status label and collapsible password widget
                        // switch to welcome page
                        if (controlPanelStackView.currentIndex
                                == controlPanelStackView.createAccountPageId
                                || controlPanelStackView.currentIndex
                                == controlPanelStackView.createSIPAccountPageId
                                || controlPanelStackView.currentIndex
                                == controlPanelStackView.importFromBackupPageId
                                || controlPanelStackView.currentIndex
                                == controlPanelStackView.importFromDevicePageId
                                || controlPanelStackView.currentIndex
                                == controlPanelStackView.connectToAccountManagerPageId) {
                            controlPanelStackView.currentIndex
                                    = controlPanelStackView.welcomePageStackId
                        }
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                RoundButton {
                    id: btnBack
                    Layout.alignment: Qt.AlignLeft
                    width: 85
                    height: 30
                    radius: height / 2

                    Layout.preferredWidth: 85
                    Layout.preferredHeight: 30
                    text: qsTr("Back")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }
                }

                Item {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillHeight: true
                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                RoundButton {
                    id: btnNext
                    Layout.alignment: Qt.AlignRight
                    width: 85
                    height: 30
                    radius: height / 2

                    Layout.minimumWidth: 85
                    Layout.preferredWidth: 85
                    Layout.maximumWidth: 85

                    Layout.minimumHeight: 30
                    Layout.preferredHeight: 30
                    Layout.maximumHeight: 30

                    text: qsTr("Next")
                    font.pointSize: 10
                    font.kerning: true

                    background: Rectangle {
                        anchors.fill: parent
                        radius: parent.radius
                        color: parent.hovered ? JamiTheme.hoverColor : JamiTheme.releaseColor
                    }
                }

                Item {
                    // put a spacer to make the buttons closs to the middle
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
    }
}
