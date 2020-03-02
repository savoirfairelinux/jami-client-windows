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

import "../commoncomponents"
import "../constant"
import "components"

Window {
    id: wizardViewWindow

    enum Mode {
        CREATE,
        IMPORT,
        MIGRATE,
        CREATESIP,
        CONNECTMANAGER
    }

    enum NameRegistrationState {
        BLANK,
        INVALID,
        TAKEN,
        FREE,
        SEARCHING
    }

    property int layoutWidth: 401
    property int layoutHeight: 625
    property int textFontSize: 9
    property int wizardMode: WizardView.CREATE
    property bool registrationStateOk: false
    property var inputParaObject: ({})
    property string fileToImport: ""

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
                    createAccountPage.nameRegistrationUIState = WizardView.SEARCHING
                    NameDirectory.lookupName("",
                                             inputParaObject["registeredName"])
                }
            }
        }
        Connections {
            target: createAccountPage

            onText_usernameEditAliasChanged: {
                registrationStateOk = false
                if (createAccountPage.checkState_signUpCheckboxAlias
                        && (createAccountPage.text_usernameEditAlias.length != 0)) {
                    inputParaObject["registeredName"] = utilsAdapter.stringSimplifier(
                                createAccountPage.text_usernameEditAlias)
                    lookupTimer.restart()
                } else {
                    createAccountPage.nameRegistrationUIState = WizardView.BLANK
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

        PasswordDialog {
            id: passwordDialog

            visible: false
            purpose: PasswordDialog.ExportAccount

            onAccepted: {
                newWizardViewQmlObjectHolder.exportToFile(
                            LrcGeneralAdapter.getCurrAccId(),
                            foldDir + "/export.gz", "")
            }

            onSuccessSignal: {
                var title = success ? qsTr("Success") : qsTr("Error")
                var info = success ? qsTr("Export Successful") : qsTr(
                                         "Export Failed")

                utilsAdapter.passwordSetStatusMessageBox(success)
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
                if (accepted) {
                    // is there password? If so, go to password dialog, else, go to following directly
                    if (newWizardViewQmlObjectHolder.hasPassword()) {
                        passwordDialog.path = foldDir + "/export.gz"
                        Helper.oneShotConnect(passwordDialog.successSignal,
                                              function (success) {})
                        passwordDialog.open()
                    } else {
                        if (folderDir.length > 0) {
                            newWizardViewQmlObjectHolder.exportToFile(
                                        LrcGeneralAdapter.getCurrAccId(),
                                        foldDir + "/export.gz", "")
                        }
                    }
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
                fileToImport = utilsAdapter.toNativeSeparators(fileDir)
                inputParaObject[""]

                if (fileToImport.length != 0) {
                    importFromBackupPage.fileImportBtnText = utilsAdapter.toFileInfoName(
                                fileToImport)
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

            onAccountAdded: {
                if (showBackUp) {
                    changePageQML(backupKeysPageId)
                } else {
                    showMainViewWindow()
                    LRCInstance.accountListChanged()
                }

                showMainViewWindow()
            }

            onShowMainViewWindow: {
                wizardViewWindow.wizardViewWindowNeedToShowMainViewWindow()
            }

            // reportFailure
            onReportFailure: {
                reportFailureQML()
            }

            function reportFailureQML() {
                spinnerPage.successState = false
                failureRedirectPageTimer.restart()
            }

            function createAccountQML() {
                console.log("Creating accounting")

                var isCreating = (wizardMode == WizardView.CREATE)

                switch (wizardMode) {
                case WizardView.CONNECTMANAGER:
                    createJAMSAccount(inputParaObject)
                    break
                case WizardView.CREATE:
                case WizardView.IMPORT:
                    createJamiAccount(inputParaObject, isCreating)
                    break
                default:
                    createSIPAccount(inputParaObject)
                }
            }

            function slotRegisteredNameFound(status, address, name) {
                if (name.length < 3) {
                    registrationStateOk = false
                    createAccountPage.nameRegistrationUIState = WizardView.INVALID
                } else if (inputParaObject["registeredName"] === name) {
                    switch (status) {
                    case NameDirectory.LookupStatus.NOT_FOUND:
                    case NameDirectory.LookupStatus.ERROR:
                        registrationStateOk = true
                        createAccountPage.nameRegistrationUIState = WizardView.FREE
                        break
                    case NameDirectory.LookupStatus.INVALID_NAME:
                    case NameDirectory.LookupStatus.INVALID:
                        registrationStateOk = false
                        createAccountPage.nameRegistrationUIState = WizardView.INVALID
                        break
                    case NameDirectory.LookupStatus.SUCCESS:
                        registrationStateOk = false
                        createAccountPage.nameRegistrationUIState = WizardView.TAKEN
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
                inputParaObject = {}
                switch (wizardMode) {
                case WizardView.CREATE:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Generating your Jami account...")
                    inputParaObject["alias"] = createAccountPage.text_fullNameEditAlias

                    inputParaObject["password"] = createAccountPage.text_confirmPasswordEditAlias

                    createAccountPage.clearAllTextFields()
                    break
                case WizardView.IMPORT:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Importing account archive...")
                    // should only work in import from backup page or import from device page
                    if (controlPanelStackView.currentIndex
                            == controlPanelStackView.importFromBackupPageId) {
                        inputParaObject["password"]
                                = importFromBackupPage.text_passwordFromDeviceAlias
                        importFromBackupPage.clearAllTextFields()
                    } else if (controlPanelStackView.currentIndex
                               == controlPanelStackView.importFromDevicePageId) {
                        inputParaObject["archivePin"] = importFromBackupPage.text_pinFromDeviceAlias
                        inputParaObject["password"]
                                = importFromDevicePage.text_passwordFromDeviceAlias
                        importFromDevicePage.clearAllTextFields()
                    }
                    break
                case WizardView.MIGRATE:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Migrating your Jami account...")
                    break
                case WizardView.CREATESIP:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Generating your SIP account...")
                    if (createSIPAccountPage.text_sipFullNameEditAlias.length == 0) {
                        inputParaObject["alias"] = "SIP"
                    } else {
                        inputParaObject["alias"] = createSIPAccountPage.text_sipFullNameEditAlias
                    }

                    inputParaObject["hostname"] = createSIPAccountPage.text_sipServernameEditAlias
                    inputParaObject["username"] = createSIPAccountPage.text_sipUsernameEditAlias
                    inputParaObject["password"] = createSIPAccountPage.text_sipPasswordEditAlias
                    inputParaObject["proxy"] = createSIPAccountPage.text_sipProxyEditAlias

                    break
                case WizardView.CONNECTMANAGER:
                    spinnerPage.progressLabelEditText = qsTr(
                                "Connecting to account manager...")
                    inputParaObject["username"]
                            = connectToAccountManagerPage.text_usernameManagerEditAlias
                    inputParaObject["password"]
                            = connectToAccountManagerPage.text_passwordManagerEditAlias
                    inputParaObject["manager"]
                            = connectToAccountManagerPage.text_accountManagerEditAlias
                    connectToAccountManagerPage.clearAllTextFields()
                    break
                }

                inputParaObject["archivePath"] = fileToImport

                if (!("archivePin" in inputParaObject)) {
                    inputParaObject["archivePath"] = ""
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
                    fileToImport = ""
                    newWizardViewQmlObjectHolder.setNavBarVisibility(false,
                                                                     true)
                    createAccountPage.nameRegistrationUIState = WizardView.BLANK
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
                    createAccountPage.nameRegistrationUIState = WizardView.BLANK
                    createAccountPage.isToSetPassword_checkState_choosePasswordCheckBox = false
                } else if (pageIndex == controlPanelStackView.connectToAccountManagerPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    connectToAccountManagerPage.initializeOnShowUp()
                    btnNext.enabled = false
                } else if (pageIndex == controlPanelStackView.importFromBackupPageId) {
                    newWizardViewQmlObjectHolder.setNavBarVisibility(true)
                    importFromBackupPage.clearAllTextFields()
                    fileToImport = ""
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
                    var validImport = !(fileToImport.length == 0)
                    btnNext.enabled = validImport
                    return
                }

                var usernameOk = !createAccountPage.checkState_signUpCheckboxAlias
                        || (createAccountPage.checkState_signUpCheckboxAlias
                            && !(inputParaObject["registeredName"].length == 0)
                            && (inputParaObject["registeredName"]
                                == createAccountPage.text_usernameEditAlias)
                            && (registrationStateOk == true))
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
                        createAccountPage.nameRegistrationUIState = WizardView.BLANK
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
                            wizardMode = WizardView.CREATE
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.importFromDevicePageId) {
                            wizardMode = WizardView.IMPORT
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.createSIPAccountPageId) {
                            wizardMode = WizardView.CREATESIP
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.connectToAccountManagerPageId) {
                            wizardMode = WizardView.CONNECTMANAGER
                            newWizardViewQmlObjectHolder.processWizardInformationsQML()
                        } else if (controlPanelStackView.currentIndex
                                   == controlPanelStackView.importFromBackupPageId) {
                            wizardMode = WizardView.IMPORT
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
