import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import net.jami.LrcGeneralAdapter 1.0
import net.jami.NewWizardViewQmlObjectHolder 1.0
import net.jami.lrc.LRCInstance 1.0
import lrc.api.classes.NewAccountModel 1.0
import net.jami.lrc.NameDirectory 1.0
import net.jami.UtilsAdapter 1.0
import lrc.api.namespaces 1.0

import "js/wizardviewjshelpers.js" as Helper

import "../constant"
import "components"

Window {
    id: wizardViewWindow

    property int layoutWidth: 401
    property int layoutHeight: 625
    property int textFontSize: 9

    signal wizardViewWindowNeedToShowMainViewWindow

    title: "Jami"
    visible: true
    width: layoutWidth
    height: layoutHeight

    Component.onCompleted: {
        newWizardViewQmlObjectHolder.setNewWizardViewQmlObject(wizardViewWindow)

        newWizardViewQmlObjectHolder.changePageQML(
                    controlPanelStackView.welcomePageStackId)
    }

    Item {
        id: newWizardViewQmlObjectHolderWrapper

        UtilsAdapter {
            id: utilsAdapter
        }

        Connections {
            id: registeredNameFoundConnection
            target: NameDirectory
            enabled: false

            onRegisteredNameFound: {
                newWizardViewQmlObjectHolder.slotRegisteredNameFound(status,
                                                                     address,
                                                                     name)
            }
        }

        // welcome page connections
        Connections {
            target: welcomePage

            onWelcomePageRedirectPage: {
                newWizardViewQmlObjectHolder.changePageQML(toPageIndex)
            }
        }

        //signal connections for create account page
        Timer {
            id: lookupTimer

            repeat: false
            triggeredOnStart: false
            interval: 200

            onTriggered: {
                if (createAccountPage.checkState_signUpCheckboxAlias
                        && (createAccountPage.text_usernameEditAlias.length != 0)) {
                    createAccountPage.nameRegistrationUIState
                            = NewWizardViewQmlObjectHolder.NameRegistrationUIState.SEARCHING
                    NameDirectory.lookupName(
                                "", newWizardViewQmlObjectHolder.registeredName)
                }
            }
        }
        Connections {
            target: createAccountPage

            onText_usernameEditAliasChanged: {
                newWizardViewQmlObjectHolder.registrationStateOk = false
                if (createAccountPage.checkState_signUpCheckboxAlias
                        && (createAccountPage.text_usernameEditAlias.length != 0)) {
                    newWizardViewQmlObjectHolder.registeredName = utilsAdapter.stringSimplifier(
                                createAccountPage.text_usernameEditAlias)
                    lookupTimer.restart()
                } else {
                    createAccountPage.nameRegistrationUIState
                            = NewWizardViewQmlObjectHolder.NameRegistrationUIState.BLANK
                    lookupTimer.stop()
                    if (createAccountPage.text_usernameEditAlias.length == 0) {
                        lookupTimer.restart()
                    }
                }
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onValidateWizardProgressionCreateAccountPage: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onText_passwordEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onText_confirmPasswordEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }
        }

        // signal connections for back up key page
        Connections {
            target: backupKeysPage

            onNeverShowAgainBoxClicked: {
                newWizardViewQmlObjectHolder.settingsNeverShowAgainBox(
                            isChecked)
            }

            onExport_Btn_FileDialogAccepted: {
                if (folderDir.length > 0) {
                    LrcGeneralAdapter.accountModel().exportToFile(
                                LrcGeneralAdapter.getCurrAccId(),
                                folderDir + "/export.gz")
                }
                newWizardViewQmlObjectHolder.showMainViewWindow()
                LRCInstance.accountListChanged()
            }

            onSkip_Btn_Clicked: {
                newWizardViewQmlObjectHolder.showMainViewWindow()
                LRCInstance.accountListChanged()
            }
        }

        // signal connections for import from backup page
        Connections {
            target: importFromBackupPage

            onText_passwordFromBackupEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onImportFromFile_Dialog_Accepted: {
                newWizardViewQmlObjectHolder.fileToImport = utilsAdapter.toNativeSeparators(
                            fileDir)
                if (newWizardViewQmlObjectHolder.fileToImport.length != 0) {
                    importFromBackupPage.fileImportBtnText = utilsAdapter.toFileInfoName(
                                newWizardViewQmlObjectHolder.fileToImport)
                } else {
                    importFromBackupPage.fileImportBtnText = qsTr(
                                "Archive(none)")
                }
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }
        }

        // signal connections for import from device page
        Connections {
            target: importFromDevicePage

            onText_pinFromDeviceAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onText_passwordFromDeviceAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }
        }

        // signal connections for connect account manader page
        Connections {
            target: connectToAccountManagerPage

            onText_usernameManagerEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onText_passwordManagerEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }

            onText_accountManagerEditAliasChanged: {
                newWizardViewQmlObjectHolder.validateWizardProgressionQML()
            }
        }

        // redirect to mainview timer
        Timer {
            id: redirectToMainViewTimer

            repeat: false
            triggeredOnStart: false
            interval: 2000

            onTriggered: {
                newWizardViewQmlObjectHolder.showMainViewWindow()
                LRCInstance.accountListChanged()
            }

            function redirectMain(timeInSecond) {
                interval = timeInSecond * 1000
                redirectToMainViewTimer.restart()
            }
        }

        // failure redirect timer and qml object holder
        Timer {
            id: failureRedirectPageTimer

            repeat: false
            triggeredOnStart: false
            interval: 1000

            onTriggered: {
                spinnerPage.successState = true
            }
        }

        NewWizardViewQmlObjectHolder {
            id: newWizardViewQmlObjectHolder

            property var inputParaObject: ({})

            onShowMainViewWindow: {
                wizardViewWindow.wizardViewWindowNeedToShowMainViewWindow()
            }

            onUpdateQuickWindow: {
                wizardViewWindow.update()
            }

            // reportFailure
            onReportFailure: {
                reportFailureQML()
            }

            // change page
            onChangePage: {
                changePageQML(pageIndex)
            }

            function reportFailureQML() {
                spinnerPage.successState = false
                failureRedirectPageTimer.restart()
            }

            function createAccountQML() {
                console.log("Creating accounting")

                var isConnectingToManager = (wizardMode == NewWizardViewQmlObjectHolder.WizardMode.CONNECTMANAGER)
                var isRing = (wizardMode == NewWizardViewQmlObjectHolder.WizardMode.CREATE
                              || wizardMode == NewWizardViewQmlObjectHolder.WizardMode.IMPORT)
                var isCreating = (wizardMode == NewWizardViewQmlObjectHolder.WizardMode.CREATE)

                newWizardViewQmlObjectHolder.createAccount(
                            isConnectingToManager, isRing, isCreating)
            }

            function slotRegisteredNameFound(status, address, name) {
                if (name.length < 3) {
                    newWizardViewQmlObjectHolder.registrationStateOk = false
                    createAccountPage.nameRegistrationUIState
                            = NewWizardViewQmlObjectHolder.NameRegistrationUIState.INVALID
                } else if (newWizardViewQmlObjectHolder.registeredName === name) {
                    switch (status) {
                    case NameDirectory.LookupStatus.NOT_FOUND:
                    case NameDirectory.LookupStatus.ERROR:
                        registrationStateOk = true
                        createAccountPage.nameRegistrationUIState
                                = NewWizardViewQmlObjectHolder.NameRegistrationUIState.FREE
                        break
                    case NameDirectory.LookupStatus.INVALID_NAME:
                    case NameDirectory.LookupStatus.INVALID:
                        registrationStateOk = false
                        createAccountPage.nameRegistrationUIState
                                = NewWizardViewQmlObjectHolder.NameRegistrationUIState.INVALID
                        break
                    case NameDirectory.LookupStatus.SUCCESS:
                        registrationStateOk = false
                        createAccountPage.nameRegistrationUIState
                                = NewWizardViewQmlObjectHolder.NameRegistrationUIState.TAKEN
                        break
                    }
                }
                validateWizardProgressionQML()
            }

            // function to set up nav bar visibility and the three buttons' visibiliy
            function setNavBarVisibility(navVisible, back) {
                navBarView.visible = (navVisible == true) || (back == true)
                btnNext.visible = (navVisible == true)
                btnPevious.visible = (navVisible == true)
                btnBack.visible = (back == true)
                        && (LrcGeneralAdapter.getAccountListSize() != 0)
            }

            function processWizardInformationsQML() {
                newWizardViewQmlObjectHolder.inputParaObject = {}
                switch (wizardMode) {
                case NewWizardViewQmlObjectHolder.WizardMode.CREATE:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Generating your Jami account...")
                    newWizardViewQmlObjectHolder.inputParaObject["alias"]
                            = createAccountPage.text_fullNameEditAlias

                    newWizardViewQmlObjectHolder.inputParaObject["password"]
                            = createAccountPage.text_confirmPasswordEditAlias

                    createAccountPage.clearAllTextFields()
                    break
                case NewWizardViewQmlObjectHolder.WizardMode.IMPORT:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Importing account archive...")
                    // should only work in import from backup page or import from device page
                    if (controlPanelStackView.currentIndex
                            == controlPanelStackView.importFromBackupPageId) {
                        newWizardViewQmlObjectHolder.inputParaObject["password"]
                                = importFromBackupPage.text_passwordFromDeviceAlias
                        importFromBackupPage.clearAllTextFields()
                    } else if (controlPanelStackView.currentIndex
                               == controlPanelStackView.importFromDevicePageId) {
                        newWizardViewQmlObjectHolder.inputParaObject["archivePin"]
                                = importFromBackupPage.text_pinFromDeviceAlias
                        newWizardViewQmlObjectHolder.inputParaObject["password"]
                                = importFromDevicePage.text_passwordFromDeviceAlias
                        importFromDevicePage.clearAllTextFields()
                    }
                    break
                case NewWizardViewQmlObjectHolder.WizardMode.MIGRATE:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Migrating your Jami account...")
                    break
                case NewWizardViewQmlObjectHolder.WizardMode.CREATESIP:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Generating your SIP account...")
                    if (createSIPAccountPage.text_sipFullNameEditAlias.length == 0) {
                        newWizardViewQmlObjectHolder.inputParaObject["alias"] = "SIP"
                    } else {
                        newWizardViewQmlObjectHolder.inputParaObject["alias"]
                                = createSIPAccountPage.text_sipFullNameEditAlias
                    }

                    newWizardViewQmlObjectHolder.inputParaObject["hostname"]
                            = createSIPAccountPage.text_sipServernameEditAlias
                    newWizardViewQmlObjectHolder.inputParaObject["username"]
                            = createSIPAccountPage.text_sipUsernameEditAlias
                    newWizardViewQmlObjectHolder.inputParaObject["password"]
                            = createSIPAccountPage.text_sipPasswordEditAlias
                    newWizardViewQmlObjectHolder.inputParaObject["proxy"]
                            = createSIPAccountPage.text_sipProxyEditAlias

                    break
                case NewWizardViewQmlObjectHolder.WizardMode.CONNECTMANAGER:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Connecting to account manager...")
                    newWizardViewQmlObjectHolder.inputParaObject["username"]
                            = connectToAccountManagerPage.text_usernameManagerEditAlias
                    newWizardViewQmlObjectHolder.inputParaObject["password"]
                            = connectToAccountManagerPage.text_passwordManagerEditAlias
                    newWizardViewQmlObjectHolder.inputParaObject["manager"]
                            = connectToAccountManagerPage.text_accountManagerEditAlias
                    connectToAccountManagerPage.clearAllTextFields()
                    break
                }

                newWizardViewQmlObjectHolder.inputParaObject["archivePath"] = fileToImport

                if (!("archivePin" in newWizardViewQmlObjectHolder.inputParaObject)) {
                    newWizardViewQmlObjectHolder.inputParaObject["archivePath"] = ""
                }

                // sync input para
                newWizardViewQmlObjectHolder.clearInputPara()
                for (var key in newWizardViewQmlObjectHolder.inputParaObject) {
                    console.log("Sync Input para, Key: " + key + " value: "
                                + newWizardViewQmlObjectHolder.inputParaObject[key])

                    newWizardViewQmlObjectHolder.modifyInputPara(
                                key,
                                newWizardViewQmlObjectHolder.inputParaObject[key])
                }

                // change page to spinner page
                changePageQML(controlPanelStackView.spinnerPageId)
                //create account
                createAccountQML()
                utilsAdapter.createStartupLink()
            }

            function changePageQML(pageIndex) {
                if (pageIndex == controlPanelStackView.spinnerPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(false)
                }
                controlPanelStackView.currentIndex = pageIndex
                if (pageIndex == controlPanelStackView.welcomePageStackId) {
                    newWizardViewQmlObjectHolder.fileToImport = ""
                    newWizardViewQmlObjectHolder.setNavBarVisibility(false,
                                                                     true)
                    createAccountPage.nameRegistrationUIState
                            = NewWizardViewQmlObjectHolder.NameRegistrationUIState.BLANK
                } else if (pageIndex == controlPanelStackView.createAccountPageId) {
                    createAccountPage.initializeOnShowUp()
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    // connection between register name found and its slot
                    registeredNameFoundConnection.enabled = true
                    // validate wizard progression
                    validateWizardProgressionQML()
                    // start photobooth
                } else if (pageIndex == controlPanelStackView.createSIPAccountPageId) {
                    createSIPAccountPage.initializeOnShowUp()
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    btnNext.enabled = true
                    // start photo booth
                } else if (pageIndex == controlPanelStackView.importFromDevicePageId) {
                    importFromDevicePage.initializeOnShowUp()
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                } else if (pageIndex == controlPanelStackView.spinnerPageId) {
                    createAccountPage.nameRegistrationUIState
                            = NewWizardViewQmlObjectHolder.NameRegistrationUIState.BLANK
                    createAccountPage.isToSetPassword_checkState_choosePasswordCheckBox = false
                } else if (pageIndex == controlPanelStackView.connectToAccountManagerPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    connectToAccountManagerPage.initializeOnShowUp()
                    btnNext.enabled = false
                } else if (pageIndex == controlPanelStackView.importFromBackupPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    importFromBackupPage.clearAllTextFields()
                    newWizardViewQmlObjectHolder.fileToImport = ""
                    btnNext.enabled = false
                } else if (pageIndex == controlPanelStackView.backupKeysPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(false)
                }
            }

            function validateWizardProgressionQML() {
                if (controlPanelStackView.currentIndex
                        == controlPanelStackView.importFromDevicePageId) {
                    var validPin = !(importFromDevicePage.text_pinFromDeviceAlias.length == 0)
                    btnNext.enabled = validPin
                    return
                } else if (controlPanelStackView.currentIndex
                           == controlPanelStackView.connectToAccountManagerPageId) {
                    var validUsername = !(connectToAccountManagerPage.text_usernameManagerEditAlias.length == 0)
                    var validPassword = !(connectToAccountManagerPage.text_passwordManagerEditAlias.length == 0)
                    var validManager = !(connectToAccountManagerPage.text_accountManagerEditAlias.length == 0)
                    btnNext.enabled = validUsername && validPassword
                            && validManager
                    return
                } else if (controlPanelStackView.currentIndex
                           == controlPanelStackView.importFromBackupPageId) {
                    var validImport = !(newWizardViewQmlObjectHolder.fileToImport.length == 0)
                    btnNext.enabled = validImport
                    return
                }

                var usernameOk = !createAccountPage.checkState_signUpCheckboxAlias
                        || (createAccountPage.checkState_signUpCheckboxAlias
                            && !(newWizardViewQmlObjectHolder.registeredName.length == 0)
                            && (newWizardViewQmlObjectHolder.registeredName
                                == createAccountPage.text_usernameEditAlias)
                            && (newWizardViewQmlObjectHolder.registrationStateOk == true))
                var passwordOk = (createAccountPage.text_passwordEditAlias
                                  == createAccountPage.text_confirmPasswordEditAlias)

                // set password status label
                if (passwordOk
                        && !(createAccountPage.text_passwordEditAlias.length == 0)) {
                    createAccountPage.displayState_passwordStatusLabelAlias = "Success"
                } else if (!passwordOk) {
                    createAccountPage.displayState_passwordStatusLabelAlias = "Fail"
                } else {
                    createAccountPage.displayState_passwordStatusLabelAlias = "Hide"
                }
                //set enable state of next button
                btnNext.enabled = (usernameOk && passwordOk)
            }
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
                    }

                    CreateSIPAccountPage {
                        // create SIP account page, index 2
                        id: createSIPAccountPage
                    }

                    ImportFromBackupPage {
                        // import from backup page, index 3
                        id: importFromBackupPage
                    }

                    BackupKeyPage {
                        // backup keys page, index 4
                        id: backupKeysPage
                    }

                    ImportFromDevicePage {
                        // import from device page, index 5
                        id: importFromDevicePage
                    }

                    ConnectToAccountManagerPage {
                        // connect to account manager Page, index 6
                        id: connectToAccountManagerPage
                    }

                    SpinnerPage {
                        // spinner Page, index 7
                        id: spinnerPage
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
                        registeredNameFoundConnection.enabled = false
                        // deal with look up status label and collapsible password widget
                        createAccountPage.nameRegistrationUIState
                                = NewWizardViewQmlObjectHolder.NameRegistrationUIState.BLANK
                        createAccountPage.isToSetPassword_checkState_choosePasswordCheckBox = false
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
                            newWizardViewQmlObjectHolder.changePageQML(
                                        controlPanelStackView.welcomePageStackId)
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

                    onClicked: {
                        newWizardViewQmlObjectHolder.showMainViewWindow()
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

                    onClicked: {
                        // TODO: stop photobooth
                        registeredNameFoundConnection.enabled = false

                        if (controlPanelStackView.currentIndex
                                == controlPanelStackView.createAccountPageId) {
                            newWizardViewQmlObjectHolder.wizardMode
                                    = NewWizardViewQmlObjectHolder.WizardMode.CREATE
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.importFromDevicePageId) {
                            newWizardViewQmlObjectHolder.wizardMode
                                    = NewWizardViewQmlObjectHolder.WizardMode.IMPORT
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.createSIPAccountPageId) {
                            newWizardViewQmlObjectHolder.wizardMode
                                    = NewWizardViewQmlObjectHolder.WizardMode.CREATESIP
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.connectToAccountManagerPageId) {
                            newWizardViewQmlObjectHolder.wizardMode
                                    = NewWizardViewQmlObjectHolder.WizardMode.CONNECTMANAGER
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.importFromBackupPageId) {
                            newWizardViewQmlObjectHolder.wizardMode
                                    = NewWizardViewQmlObjectHolder.WizardMode.IMPORT
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        }
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
