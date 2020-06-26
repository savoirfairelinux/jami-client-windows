import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "mainview"
import "wizardview"
import "settingsview"

ApplicationWindow {
    id: mainApplicationWindow

    Universal.theme: Universal.Light

    visible: false

    Loader {
        id: mainViewLoader

        //asynchronous: true
        visible: status == Loader.Ready
        source: ""

//        Connections {
//            target: mainViewLoader.item

//            function onNeedToAddNewAccount() {
//                wizardView.show()
//            }

//            function onCloseApp() {
//                Qt.quit()
//            }
//        }
    }

    Component{
        id: mainViewComponent

        MainView{
            id: mainView

            onNeedToAddNewAccount: {
                wizardView.show()
            }

            onCloseApp: {
                Qt.quit()
            }

            onMainViewWindowNeedToShowSettingsViewWindow:{
                mainViewLoader.sourceComponent = settingsViewComponent
            }
        }
    }

    Component{
        id: settingsViewComponent

        SettingsView{
            id: settingsView

            onSettingsViewWindowNeedToShowMainViewWindow:{
                mainViewLoader.sourceComponent = mainViewComponent
            }
        }
    }

    WizardView {
        id: wizardView

//        onNeedToShowMainViewWindow: {
//            if (mainViewLoader.source.toString() !== "qrc:/src/mainview/MainView.qml")
//                mainViewLoader.setSource("qrc:/src/mainview/MainView.qml")
//            if(accountIndex !== -1)
//                mainViewLoader.item.newAccountAdded(accountIndex)
//        }

//        onWizardViewIsClosed: {
//            if (mainViewLoader.source.toString() !== "qrc:/src/mainview/MainView.qml") {
//                Qt.quit()
//            }
//        }

        onNeedToShowMainViewWindow: {
            if (mainViewLoader.sourceComponent !== mainViewComponent)
                mainViewLoader.sourceComponent = mainViewComponent
            if(accountIndex !== -1)
                mainViewLoader.item.newAccountAdded(accountIndex)
        }

        onWizardViewIsClosed: {
            if (mainViewLoader.sourceComponent !== mainViewComponent) {
                Qt.quit()
            }
        }
    }

    Component.onCompleted: {
        setX(Screen.width / 2 - width / 2)
        setY(Screen.height / 2 - height / 2)

        if (!ClientWrapper.utilsAdaptor.getAccountListSize()) {
            wizardView.show()
        } else {
            //.setSource("qrc:/src/mainview/MainView.qml")
            mainViewLoader.sourceComponent = settingsViewComponent
        }
    }

    overlay.modal: ColorOverlay {
        source: mainApplicationWindow.contentItem
        color: "transparent"


        /*
         * Color animation for overlay when pop up is shown.
         */
        ColorAnimation on color {
            to: Qt.rgba(0, 0, 0, 0.33)
            duration: 500
        }
    }
}
