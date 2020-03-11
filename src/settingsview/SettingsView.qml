import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 1.4 as CT
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14
import net.jami.SettingsViewQmlObjectHolder 1.0
import net.jami.tools.utils 1.0

import "components"

Window {
    id: settingsViewWindow

    property int minWidth: 768
    property int minHeight: 768
    property int textFontSize: 9

    /*
     * signal to redirect the page to main view
     */
    signal settingsViewWindowNeedToShowMainViewWindow

    title: "Jami"
    visible: true
    width: 768
    height: 768
    minimumWidth: minWidth
    minimumHeight: minHeight

    Rectangle {
        id: settingsViewRect
        anchors.fill: parent

        // TODO: hook up "isSIP" with LRC call
        property bool isSIP: false

        CT.SplitView {
            anchors.fill: parent
            orientation: Qt.Horizontal

            Rectangle {
                id: leftSettingsWidget

                Layout.minimumWidth: 380
                Layout.preferredWidth: 380
                Layout.maximumWidth: parent.width / 2
                Layout.fillHeight: true
                LeftPanelView {
                    id: leftPanelView

                    function setRightPageRightSettingStack() {
                        if (leftPanelView.btnAccountSettings.checked) {
                            if (settingsViewRect.isSIP) {
                                rightSettingsWidget.currentIndex = 1
                            } else {
                                rightSettingsWidget.currentIndex = 0
                            }
                        } else if (leftPanelView.btnGeneralSettings.checked) {
                            rightSettingsWidget.currentIndex = 2
                        } else if (leftPanelView.btnMediaSettings.checked) {
                            rightSettingsWidget.currentIndex = 3
                        }
                    }
                    Connections {
                        target: leftPanelView.btnAccountSettings
                        onCheckedToggledForRightPanel: {
                            leftPanelView.setRightPageRightSettingStack()
                        }
                    }
                    Connections {
                        target: leftPanelView.btnGeneralSettings
                        onCheckedToggledForRightPanel: {
                            leftPanelView.setRightPageRightSettingStack()
                        }
                    }
                    Connections {
                        target: leftPanelView.btnMediaSettings
                        onCheckedToggledForRightPanel: {
                            leftPanelView.setRightPageRightSettingStack()
                        }
                    }
                }
            }

            StackLayout {
                id: rightSettingsWidget

                property int pageIdCurrentAccountSettingsScrollPage: 0
                property int pageIdCurrentSIPAccountSettingScrollPage: 1
                property int pageIdGeneralSettingsPage: 2
                property int pageIdAvSettingPage: 3

                currentIndex: settingsViewRect.isSIP ? 0 : 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                // current account setting scroll page, index 0
                CurrentAccountSettingsScrollPage {
                    id: currentAccountSettingsScrollWidget
                }

                // current SIP account setting scroll page, index 1
                CurrentSIPAccountSettingScrollPage {
                    id: currentSIPAccountSettingsScrollWidget
                }

                // general setting page, index 2
                GeneralSettingsPage {
                    id: generalSettings
                }

                // av setting page, index 3
                AvSettingPage {
                    id: avSettings
                }
            }
        }
    }
}
