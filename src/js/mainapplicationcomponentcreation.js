var settingsViewWindowComponent
var settingsViewWindowObject

var wizardViewWindowComponent
var wizardViewWindowObject

var mainViewWindowComponent
var mainViewWindowObject

/*################################################################################
# SettingsViewWindow
################################################################################*/

function createSettingsViewWindowObjects() {
    settingsViewWindowComponent = Qt.createComponent(
                "../settingsview/SettingsView.qml")
    if (settingsViewWindowComponent.status === Component.Ready)
        createSettingsViewWindowFinishCreation()
    else
        settingsViewWindowComponent.statusChanged.connect(createSettingsViewWindowFinishCreation)
}

function createSettingsViewWindowFinishCreation() {
    if (settingsViewWindowComponent.status === Component.Ready) {
        settingsViewWindowObject = settingsViewWindowComponent.createObject()
        if (settingsViewWindowObject === null) {
            // Error Handling
            console.log("Error creating object for settings view")
        }

        settingsViewWindowObject.settingsViewWindowNeedToShowMainViewWindow.connect(createMainViewWindowObjects)
        settingsViewWindowObject.settingsViewWindowNeedToShowMainViewWindow.connect(closeSettingsViewWindow)
    } else if (settingsViewWindowComponent.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:",
                    settingsViewWindowComponent.errorString())
    }
}

function showSettingsViewWindow() {
    settingsViewWindowObject.show()
}

function closeSettingsViewWindow() {
    settingsViewWindowObject.close()
    settingsViewWindowObject.destroy()
}

/*################################################################################
# WizardViewWindow
################################################################################*/

function createWizardViewWindowObjects() {
    wizardViewWindowComponent = Qt.createComponent(
                "../wizardview/WizardView.qml")
    if (wizardViewWindowComponent.status === Component.Ready)
        createWizardViewWindowFinishCreation()
    else
        wizardViewWindowComponent.statusChanged.connect(createWizardViewWindowFinishCreation)
}

function createWizardViewWindowFinishCreation() {
    if (wizardViewWindowComponent.status === Component.Ready) {
        wizardViewWindowObject = wizardViewWindowComponent.createObject()
        if (wizardViewWindowObject === null) {
            // Error Handling
            console.log("Error creating object for wizard view")
        }

        wizardViewWindowObject.wizardViewWindowNeedToShowMainViewWindow.connect(createMainViewWindowObjects)
        wizardViewWindowObject.wizardViewWindowNeedToShowMainViewWindow.connect(closeWizardViewWindow)
    } else if (wizardViewWindowComponent.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:",
                    wizardViewWindowComponent.errorString())
    }
}

function showWizardViewWindow() {
    wizardViewWindowObject.show()
}

function closeWizardViewWindow() {
    wizardViewWindowObject.close()
    wizardViewWindowObject.destroy()
}

/*################################################################################
# MainViewWindow
################################################################################*/

function createMainViewWindowObjects() {
    mainViewWindowComponent = Qt.createComponent(
                "../mainview/MainView.qml")
    if (mainViewWindowComponent.status === Component.Ready)
        createMainViewWindowFinishCreation()
    else
        mainViewWindowComponent.statusChanged.connect(createMainViewWindowFinishCreation)
}

function createMainViewWindowFinishCreation() {
    if (mainViewWindowComponent.status === Component.Ready) {
        mainViewWindowObject = mainViewWindowComponent.createObject()
        if (mainViewWindowObject === null) {
            // Error Handling
            console.log("Error creating object for wizard view")
        }

        //wizardViewWindowObject.showMainViewWindow.connect()
    } else if (mainViewWindowComponent.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:",
                    mainViewWindowComponent.errorString())
    }
}

function showMainViewWindow() {
    mainViewWindowObject.show()
}

function closeMainViewWindow() {
    mainViewWindowObject.close()
    mainViewWindowObject.destroy()
}
