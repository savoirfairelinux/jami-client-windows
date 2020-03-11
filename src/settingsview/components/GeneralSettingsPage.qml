import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.14
import net.jami.Models 1.0
import "../../commoncomponents"

Rectangle {
    function populateGeneralSettings(){
        // settings
        closeOrMinCheckBox.checked = isMinOnClose
        applicationOnStartUpCheckBox.checked = isApplicationOnStartUpCheckBox
        notificationCheckBox.checked = notifs

        alwaysRecordingCheckBox.checked = alwaysRecord
        recordPreviewCheckBox.checked = recordPreview
        recordQualityValueLabel.text = recordQualityStr
        recordQualitySlider.value = recordQuality

        avmodel.setRecordPath(recordPath)

        autoUpdateCheckBox.checked = autoUpdate
    }

    function slotSetNotifications(state){
        SettingsAdaptor.setNotifications(state)
    }

    function slotSetClosedOrMin(state){
        SettingsAdaptor.setClosedOrMin(state)
    }

    function slotSetRunOnStartUp(state){
        SettingsAdaptor.setRunOnStartUp(state)
    }

    function slotSetUpdateAutomatic(state){
        SettingsAdaptor.setUpdateAutomatic(state)
    }

    function slotAlwaysRecordingClicked(state){
        avmodel.setAlwaysRecord(state)
    }

    function slotRecordPreviewClicked(state){
        avmodel.setRecordPreview(state)
    }

    function slotRecordQualitySliderValueChanged(value){
        recordQualityValueLabel.text = UtilsAdapter.getRecordQualityString(value)
        updateRecordQualityTimer.restart()
    }

    Timer{
        id: updateRecordQualityTimer

        interval: 500

        onTriggered: {
            slotRecordQualitySliderSliderReleased()
        }
    }

    function slotRecordQualitySliderSliderReleased(){
        var value = recordQualitySlider.value
        avmodel.setRecordQuality(value * 100)
    }

    function openDownloadFolderSlot(){
        downloadPathDialog.open()
    }

    FileDialog {
        id: downloadPathDialog

        selectFolder: true

        title: qsTr("Select A Folder For Your Downloads")
        folder: shortcuts.home + "/Downloads"

        onAccepted: {
            var dir = JSON. stringify(folder).replace("file:///", "")
            downloadPath = dir
        }

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }
    }

    function openRecordFolderSlot(){
        recordPathDialog.open()
    }

    FileDialog {
        id: recordPathDialog

        selectFolder: true

        title: qsTr("Select A Folder For Your Recordings")
        folder: shortcuts.home

        onAccepted: {
            var dir = JSON. stringify(folder).replace("file:///", "")
            downloadPath = dir
        }

        onRejected: {}

        onVisibleChanged: {
            if (!visible) {
                rejected()
            }
        }
    }

    //TODO: complete check for update and check for Beta slot functions
    function checkForUpdateSlot(){}
    function installBetaSlot(){}

    // settings
    property string downloadPath: SettingsAdaptor.getDir_Download()
    property bool isMinOnClose: SettingsAdaptor.getSettingsValue_CloseOrMinimized()
    property bool isApplicationOnStartUpCheckBox: UtilsAdapter.checkStartupLink()
    property bool notifs: SettingsAdaptor.getSettingsValue_EnableNotifications()

    // recording
    property AVModel avmodel: AccountAdapter.avModel()

    property bool alwaysRecord: avmodel.getAlwaysRecord()
    property bool recordPreview: avmodel.getRecordPreview()
    property int recordQuality: avmodel.getRecordQuality() / 100
    property string recordQualityStr: UtilsAdapter.getRecordQualityString(recordQuality)
    property string recordPath: SettingsAdaptor.getDir_Document()
    property bool autoUpdate: SettingsAdaptor.getSettingsValue_AutoUpdate()

    onDownloadPathChanged: {
        if(downloadPath === "") return
        SettingsAdaptor.setDownloadPath(downloadPath)
    }

    onRecordPathChanged: {
        if(recordPath === "") return
        avmodel.setRecordPath(recordPath)
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillHeight: true
            Layout.maximumWidth: 48
            Layout.preferredWidth: 48
            Layout.minimumWidth: 48
        }

        ColumnLayout {
            spacing: 6

            Layout.fillHeight: true
            Layout.maximumWidth: 580
            Layout.preferredWidth: 580
            Layout.minimumWidth: 580

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 10
                Layout.preferredHeight: 10
                Layout.maximumHeight: 10
            }

            Label {
                Layout.fillWidth: true
                Layout.minimumHeight: 25
                Layout.preferredHeight: 25
                Layout.maximumHeight: 25

                text: qsTr("General")
                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 24
                Layout.preferredHeight: 24
                Layout.maximumHeight: 24
            }

            // system setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("System")
                    font.pointSize: 13
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: notificationCheckBox

                        Layout.leftMargin: 20

                        labelText: "Enable desktop notifications"
                        fontPointSize: 11

                        onCheckedChanged: {
                            slotSetNotifications(checked)
                        }
                    }

                    ToggleSwitch {
                        id: closeOrMinCheckBox

                        Layout.leftMargin: 20

                        labelText: "Keep minimize on close"
                        fontPointSize: 11

                        onCheckedChanged: {
                            slotSetClosedOrMin(checked)
                        }
                    }

                    ToggleSwitch {
                        id: applicationOnStartUpCheckBox

                        Layout.leftMargin: 20

                        labelText: "Run on Startup"
                        fontPointSize: 11

                        onCheckedChanged: {
                            slotSetRunOnStartUp(checked)
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 94
                            Layout.preferredWidth: 94
                            Layout.minimumWidth: 94

                            text: qsTr("Download folder")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        HoverableButton {
                            id: downloadButton

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            icon.source: "qrc:/images/icons/round-folder-24px.svg"
                            icon.height: 24
                            icon.width: 24

                            text: downloadPath
                            fontPointSize: 10

                            onClicked: {
                                openDownloadFolderSlot()
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 20
                Layout.preferredHeight: 20
                Layout.maximumHeight: 20
            }

            // call recording setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("Call Recording")
                    font.pointSize: 13
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: alwaysRecordingCheckBox

                        Layout.leftMargin: 20

                        labelText: "Always record calls"
                        fontPointSize: 11
                    }

                    ToggleSwitch {
                        id: recordPreviewCheckBox

                        Layout.leftMargin: 20

                        labelText: "Record preview video for a call"
                        fontPointSize: 11
                    }

                    RowLayout {
                        spacing: 6
                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 42
                            Layout.preferredWidth: 42
                            Layout.minimumWidth: 42

                            text: qsTr("Quality")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        ColumnLayout {
                            spacing: 0
                            Layout.fillHeight: true

                            Layout.maximumWidth: recordQualityValueLabel.width
                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }

                            Label {
                                id: recordQualityValueLabel

                                Layout.minimumWidth: 40

                                Layout.minimumHeight: 16
                                Layout.preferredHeight: 16
                                Layout.maximumHeight: 16

                                text: qsTr("VALUE ")

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }

                        Slider{
                            id: recordQualitySlider

                            Layout.fillHeight: true

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            from: 0
                            to: 500
                            stepSize: 1

                            onMoved: {
                                slotRecordQualitySliderValueChanged(value)
                            }
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        Label {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 42
                            Layout.preferredWidth: 42
                            Layout.minimumWidth: 42

                            text: qsTr("Save in")
                            font.pointSize: 10
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }

                        HoverableButton {
                            id: recordPathButton

                            Layout.maximumWidth: 320
                            Layout.preferredWidth: 320
                            Layout.minimumWidth: 320

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            icon.source: "qrc:/images/icons/round-folder-24px.svg"
                            icon.height: 24
                            icon.width: 24

                            text: avmodel.getRecordPath()
                            fontPointSize: 10

                            onClicked: {
                                openRecordFolderSlot()
                            }
                        }
                    }
                }
            }

            Item {
                Layout.fillWidth: true
                Layout.minimumHeight: 20
                Layout.preferredHeight: 20
                Layout.maximumHeight: 20
            }

            // update setting panel
            ColumnLayout {
                spacing: 6
                Layout.fillWidth: true

                Label {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 21
                    Layout.preferredHeight: 21
                    Layout.maximumHeight: 21

                    text: qsTr("Updates")
                    font.pointSize: 13
                    font.kerning: true

                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 10
                    Layout.preferredHeight: 10
                    Layout.maximumHeight: 10
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: autoUpdateCheckBox

                        Layout.leftMargin: 20

                        labelText: "Check for updates automatically"
                        fontPointSize: 11

                        onCheckedChanged: {
                            slotSetUpdateAutomatic(checked)
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        HoverableButton {
                            id: checkUpdateButton

                            Layout.maximumWidth: 275
                            Layout.preferredWidth: 275
                            Layout.minimumWidth: 275

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: "Check for updates now"
                            fontPointSize: 10

                            onClicked: {
                                checkForUpdateSlot()
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        spacing: 6

                        Layout.leftMargin: 20
                        Layout.fillWidth: true
                        Layout.maximumHeight: 30

                        HoverableButton {
                            id: installBetaButton

                            Layout.maximumWidth: 275
                            Layout.preferredWidth: 275
                            Layout.minimumWidth: 275

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: "Install the latest beta version"
                            fontPointSize: 10

                            onClicked: {
                                installBetaSlot()
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                        }
                    }
                }
            }

            // spacer on the bottom
            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
