import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "wizardview"
import "js/mainapplicationcomponentcreation.js" as MainApplicationComponentCreation

ApplicationWindow {
    id: mainApplicationWindow

    Universal.theme: Universal.Light

    visible: false


    /*Loader {
        id: mainViewLoader

        Component.onCompleted: {
            if(LrcGeneralAdaptor.getCurrAccList().size === 0) {
                mainViewLoader.source = "wizardview/WizardView.qml"
            } else {
                mainViewLoader.source = "mainview/MainView.qml"
            }
        }
    }*/
    Component.onCompleted: {
        MainApplicationComponentCreation.createWizardViewWindowObjects()
        MainApplicationComponentCreation.showWizardViewWindow()
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
