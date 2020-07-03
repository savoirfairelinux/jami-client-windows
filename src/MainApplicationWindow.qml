import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "mainview"
import "wizardview"

ApplicationWindow {
    id: mainApplicationWindow

    Universal.theme: Universal.Light

    visible: false

    Loader {
        id: mainViewLoader

        asynchronous: true
        visible: status == Loader.Ready
        source: ""

        Connections {
            target: mainViewLoader.item

            function onNeedToAddNewAccount() {
                wizardView.show()
            }

            function onCloseApp() {
                Qt.quit()
            }

            function onNoAccountIsAvailable() {
                mainViewLoader.setSource("")
                wizardView.show()
            }
        }
    }

    WizardView {
        id: wizardView

        onNeedToShowMainViewWindow: {
            if (mainViewLoader.source.toString() !== "qrc:/src/mainview/MainView.qml")
                mainViewLoader.setSource("qrc:/src/mainview/MainView.qml")
            if(accountIndex !== -1)
                mainViewLoader.item.newAccountAdded(accountIndex)
        }

        onWizardViewIsClosed: {
            if (mainViewLoader.source.toString() !== "qrc:/src/mainview/MainView.qml") {
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
            mainViewLoader.setSource("qrc:/src/mainview/MainView.qml")
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
