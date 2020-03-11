import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.UtilsAdapter 1.0

import "mainview"
import "wizardview"
import "settingsview"
import "js/mainapplicationcomponentcreation.js" as MainApplicationComponentCreation

ApplicationWindow {
    id: mainApplicationWindow

    Universal.theme: Universal.Light

    visible: false


    Loader {
        id: mainViewLoader

        Component.onCompleted: {
//            if(LrcGeneralAdaptor.getCurrAccList().size === 0) {
//                mainViewLoader.source = "wizardview/WizardView.qml"
//            } else {
//                mainViewLoader.source = "mainview/MainView.qml"
//            }
            mainViewLoader.sourceComponent = mainViewComponent
        }
    }
//    Component.onCompleted: {
//        MainApplicationComponentCreation.createWizardViewWindowObjects()
//        MainApplicationComponentCreation.showWizardViewWindow()
//    }

    Component{
        id: mainViewComponent
        MainView{
            id: mainView

            onMainViewWindowNeedToShowSettingsViewWindow:{
                mainViewLoader.sourceComponent = settingsViewComponent
            }
        }
    }

    Component{
        id: wizardViewComponent
        WizardView{
            id: wizardView

            onWizardViewWindowNeedToShowMainViewWindow:{
                mainViewLoader.sourceComponent = mainViewComponent
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
