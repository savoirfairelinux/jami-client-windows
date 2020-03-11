import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.Models 1.0

import "../../commoncomponents"

Rectangle {
    enum RegName {
        BLANK,
        INVALIDFORM,
        TAKEN,
        FREE,
        SEARCHING
    }

    //property alias advaceSettingVisible: advanceSettingsView.visible

    property int regNameUi: CurrentAccountSettingsScrollPage.BLANK
    property string registeredName: ""
    property bool registeredIdNeedsSet: false

    Connections {
        id: btnRegisterNameClickConnection
        target: btnRegisterName

        enabled: {
            switch (regNameUi) {
            case CurrentAccountSettingsScrollPage.FREE:
                return true
            default:
                return false
            }
        }

        function onClicked() {
            slotRegisterName()
        }
    }

    function updateAccountInfoDisplayed() {
        setAvatar()

        accountEnableCheckBox.checked = SettingsAdaptor.get_CurrentAccountInfo_Enabled()
        displayNameLineEdit.text = SettingsAdaptor.getCurrentAccount_Profile_Info_Alias()

        var showLocalAccountConfig = (SettingsAdaptor.getAccountConfig_Manageruri() === "")
        passwdPushButton.visible = showLocalAccountConfig
        btnExportAccount.visible = showLocalAccountConfig
        linkDevPushButton.visible = showLocalAccountConfig

        registeredIdNeedsSet = (SettingsAdaptor.get_CurrentAccountInfo_RegisteredName() === "")

        currentRingID.text = SettingsAdaptor.getCurrentAccount_Profile_Info_Uri()

        // update device list view
        updateAndShowDevicesSlot()

        if (advanceSettingsView.visible) {
            advanceSettingsView.updateAccountInfoDisplayedAdvance()
        }
    }

    //property var bannedContacts: contactModel.getBannedContacts()

    function connectCurrentAccount() {
        accountConnections_ContactModel.enabled = true
        accountConnections_DeviceModel.enabled = true
    }

    function disconnectAccountConnections() {
        accountConnections_ContactModel.enabled = false
        accountConnections_DeviceModel.enabled = false
    }

    function isPhotoBoothOpened() {
        return currentAccountAvatar.takePhotoState
    }

    function setAvatar() {
        currentAccountAvatar.setAvatarPixmap(
                    SettingsAdaptor.getAvatarImage_Base64(
                        currentAccountAvatar.boothWidht),
                    SettingsAdaptor.getIsDefaultAvatar())
    }

    function stopBooth() {
        currentAccountAvatar.stopBooth()
    }

    Connections {
        id: accountConnections_ContactModel
        target: contactModel

        function onModelUpdated(uri, needsSorted) {
            updateAndShowBannedContactsSlot()
        }
    }

    Connections {
        id: accountConnections_DeviceModel
        target: deviceModel

        function onDeviceAdded(id) {
            updateAndShowDevicesSlot()
        }

        function onDeviceRevoked(id, status) {
            updateAndShowDevicesSlot()
        }

        function onDeviceUpdated(id) {
            updateAndShowDevicesSlot()
        }
    }

    // slots
    function verifyRegisteredNameSlot() {
        if (SettingsAdaptor.get_CurrentAccountInfo_RegisteredName() !== "") {
            regNameUi = CurrentAccountSettingsScrollPage.BLANK
        } else {
            registeredName = UtilsAdapter.stringSimplifier(
                        currentRegisteredID.text)
            if (registeredName !== "") {
                if (UtilsAdapter.validateRegNameForm(registeredName)) {
                    regNameUi = CurrentAccountSettingsScrollPage.SEARCHING
                    lookUpLabelTimer.restart()
                } else {
                    regNameUi = CurrentAccountSettingsScrollPage.INVALIDFORM
                }
            } else {
                regNameUi = CurrentAccountSettingsScrollPage.BLANK
            }
        }
    }

    Timer {
        id: lookUpLabelTimer

        interval: 300
        onTriggered: {
            beforeNameLookup()
        }
    }

    function beforeNameLookup() {
        NameDirectory.lookupName("", registeredName)
    }

    Connections {
        target: NameDirectory
        enabled: true

        function onRegisteredNameFound(status, address, name) {
            afterNameLookup(status, name)
        }
    }

    function afterNameLookup(status, regName) {
        if (registeredName === regName && regName.length > 2) {
            switch (status) {
            case NameDirectory.LookupStatus.NOT_FOUND:
                regNameUi = CurrentAccountSettingsScrollPage.FREE
                break
            default:
                regNameUi = CurrentAccountSettingsScrollPage.TAKEN
                break
            }
        } else {
            regNameUi = CurrentAccountSettingsScrollPage.BLANK
        }
    }

    function slotRegisterName() {}

    function setAccEnableSlot(state) {
        accountModel.setAccountEnabled(UtilsAdapter.getCurrAccId(), state)
    }

    function passwordClicked() {}

    function delAccountSlot() {}

    function exportAccountSlot() {}

    function updateAndShowBannedContactsSlot() {}

    function updateAndShowDevicesSlot() {}

    function showLinkDevSlot() {}

    property ContactModel contactModel: SettingsAdaptor.getContactModel()
    property NewDeviceModel deviceModel: SettingsAdaptor.getDeviceModel()
    property NewAccountModel accountModel: AccountAdapter.accoundModel()

//    Component.onDestruction: {
//        contactModel = null
//        deviceModel = null
//        accountModel = null
//    }

    Component.onCompleted: {
        contactModel= SettingsAdaptor.getContactModel()
        deviceModel= SettingsAdaptor.getDeviceModel()
        accountModel= AccountAdapter.accoundModel()
    }

    Layout.fillHeight: true
    Layout.fillWidth: true

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true

            Layout.maximumHeight: 10
            Layout.minimumHeight: 10
            Layout.preferredHeight: 10

            Layout.alignment: Qt.AlignTop
        }

        RowLayout {
            spacing: 6

            Layout.alignment: Qt.AlignTop

            Layout.fillWidth: true
            Layout.maximumHeight: 30
            Layout.minimumHeight: 30
            Layout.preferredHeight: accountPageTitle.height

            Item {
                Layout.fillHeight: true

                Layout.maximumWidth: 30
                Layout.preferredWidth: 30
                Layout.minimumWidth: 30
            }

            Label {
                id: accountPageTitle

                Layout.preferredWidth: 117

                Layout.maximumHeight: 25
                Layout.preferredHeight: 25
                Layout.minimumHeight: 25

                text: qsTr("Jami Account")

                font.pointSize: 15
                font.kerning: true

                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        ScrollView {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            font.pointSize: 8
            font.kerning: true
            clip: true

            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: 625

                Item {
                    Layout.fillHeight: true

                    Layout.maximumWidth: 30
                    Layout.preferredWidth: 30
                    Layout.minimumWidth: 30
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.leftMargin: 30

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 24
                        Layout.preferredWidth: 24
                        Layout.minimumWidth: 24
                    }

                    ToggleSwitch {
                        id: accountEnableCheckBox

                        labelText: qsTr("Enable")
                        fontPointSize: 11

                        onSwitchToggled: {
                            setAccEnableSlot(checked)
                        }
                    }

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.minimumWidth: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 21
                            Layout.preferredHeight: 21
                            Layout.minimumHeight: 21

                            text: qsTr("Profile")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 10
                            Layout.preferredHeight: 10
                            Layout.minimumHeight: 10
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            layoutDirection: Qt.LeftToRight

                            spacing: 6

                            PhotoboothView {
                                id: currentAccountAvatar

                                Layout.alignment: Qt.AlignHCenter

                                Layout.maximumWidth: 261
                                Layout.preferredWidth: 261
                                Layout.minimumWidth: 261
                                Layout.maximumHeight: 261
                                Layout.preferredHeight: 261
                                Layout.minimumHeight: 261

                                Layout.leftMargin: 20

                                onImageAcquired: {
                                    SettingsAdaptor.setCurrAccAvatar(imgBase64)
                                }

                                onImageCleared: {
                                    SettingsAdaptor.clearCurrentAvatar()
                                    setAvatar()
                                }
                            }

                            InfoLineEdit {
                                id: displayNameLineEdit

                                fieldLayoutWidth: 261

                                Layout.leftMargin: 20

                                font.pointSize: 10
                                font.kerning: true

                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter

                                onEditingFinished: {
                                    AccountAdapter.setCurrAccDisplayName(
                                                displayNameLineEdit.text)
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true

                        Layout.maximumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.minimumWidth: 20
                    }

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 21
                            Layout.preferredHeight: 21
                            Layout.minimumHeight: 21

                            text: qsTr("Identity")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 10
                            Layout.preferredWidth: 10
                            Layout.minimumWidth: 10
                        }

                        ColumnLayout {
                            spacing: 7

                            Layout.fillWidth: true

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                Layout.maximumWidth: 625

                                Label {
                                    Layout.maximumWidth: 13
                                    Layout.preferredWidth: 13
                                    Layout.minimumWidth: 13

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Id")
                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                TextField {
                                    id: currentRingID

                                    property var backgroundColor: "transparent"
                                    property var borderColor: "transparent"

                                    Layout.fillWidth: true

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    font.pointSize: 10
                                    font.kerning: true
                                    font.bold: true

                                    readOnly: true

                                    text: SettingsAdaptor.getCurrentAccount_Profile_Info_Uri()

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter

                                    background: Rectangle {
                                        anchors.fill: parent
                                        radius: 0
                                        border.color: currentRingID.borderColor
                                        border.width: 0
                                        color: currentRingID.backgroundColor
                                    }
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 32

                                Layout.leftMargin: 20

                                layoutDirection: Qt.LeftToRight

                                Label {
                                    id: lblRegisteredName

                                    Layout.maximumWidth: 127
                                    Layout.preferredWidth: 127
                                    Layout.minimumWidth: 127

                                    Layout.minimumHeight: 32
                                    Layout.preferredHeight: 32
                                    Layout.maximumHeight: 32

                                    text: qsTr("Registered name")

                                    font.pointSize: 10
                                    font.kerning: true

                                    horizontalAlignment: Text.AlignLeft
                                    verticalAlignment: Text.AlignVCenter
                                }

                                RowLayout {
                                    spacing: 6
                                    Layout.fillWidth: true
                                    Layout.maximumHeight: 30
                                    Layout.alignment: Qt.AlignVCenter

                                    TextField {
                                        id: currentRegisteredID

                                        Layout.maximumWidth: 300
                                        Layout.preferredWidth: 300
                                        Layout.minimumWidth: 300

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30

                                        placeholderText: registeredIdNeedsSet ? qsTr("Type here to register a username") : ""

                                        text: {
                                            if (!registeredIdNeedsSet)
                                                return SettingsAdaptor.get_CurrentAccountInfo_RegisteredName()
                                        }
                                        readOnly: !registeredIdNeedsSet

                                        font.pointSize: 10
                                        font.kerning: true
                                        font.bold: true

                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter

                                        background: Rectangle {
                                            anchors.fill: parent
                                            radius: registeredIdNeedsSet ? height / 2 : 0
                                            border.color: "transparent"
                                            border.width: registeredIdNeedsSet ? 2 : 0
                                            color: registeredIdNeedsSet ? Qt.rgba(
                                                                              240 / 256, 240 / 256,
                                                                              240 / 256,
                                                                              1.0) : "transparent"
                                        }

                                        onTextEdited: {
                                            verifyRegisteredNameSlot()
                                        }
                                    }

                                    LookupStatusLabel {
                                        id: lookupStatusLabel

                                        visible: registeredIdNeedsSet
                                                 && (regNameUi
                                                     !== CurrentAccountSettingsScrollPage.BLANK)

                                        MouseArea {
                                            id: lookupStatusLabelArea
                                            anchors.fill: parent
                                            property bool isHovering: false

                                            onEntered: isHovering = true
                                            onExited: isHovering = false

                                            hoverEnabled: true
                                        }

                                        ToolTip.visible: lookupStatusLabelArea.isHovering
                                        ToolTip.text: {
                                            switch (regNameUi) {
                                            case CurrentAccountSettingsScrollPage.BLANK:
                                                return qsTr("")
                                            case CurrentAccountSettingsScrollPage.INVALIDFORM:
                                                return qsTr("A registered name should not have any spaces and must be at least three letters long")
                                            case CurrentAccountSettingsScrollPage.TAKEN:
                                                return qsTr("This name is already taken")
                                            case CurrentAccountSettingsScrollPage.FREE:
                                                return qsTr("Register this name")
                                            case CurrentAccountSettingsScrollPage.SEARCHING:
                                                return qsTr("")
                                            default:
                                                return qsTr("")
                                            }
                                        }

                                        lookupStatusState: {
                                            switch (regNameUi) {
                                            case CurrentAccountSettingsScrollPage.BLANK:
                                                return "Blank"
                                            case CurrentAccountSettingsScrollPage.INVALIDFORM:
                                                return "Invalid"
                                            case CurrentAccountSettingsScrollPage.TAKEN:
                                                return "Taken"
                                            case CurrentAccountSettingsScrollPage.FREE:
                                                return "Free"
                                            case CurrentAccountSettingsScrollPage.SEARCHING:
                                                return "Searching"
                                            default:
                                                return "Blank"
                                            }
                                        }
                                    }

                                    HoverableButton {
                                        id: btnRegisterName

                                        visible: registeredIdNeedsSet
                                                 && (regNameUi
                                                     === CurrentAccountSettingsScrollPage.FREE)

                                        Layout.maximumWidth: 80
                                        Layout.preferredWidth: 80
                                        Layout.minimumWidth: 80

                                        Layout.minimumHeight: 30
                                        Layout.preferredHeight: 30
                                        Layout.maximumHeight: 30

                                        text: qsTr("Register")
                                        font.pointSize: 10
                                        font.kerning: true

                                        radius: height / 2
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                    }
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButtonTextItem {
                                    id: passwdPushButton

                                    visible: SettingsAdaptor.getAccountConfig_Manageruri() === ""

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Change Password")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2

                                    onClicked: {
                                        passwordClicked()
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButtonTextItem {
                                    id: btnExportAccount

                                    visible: SettingsAdaptor.getAccountConfig_Manageruri() === ""

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Export Account")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2

                                    onClicked: {
                                        exportAccountSlot()
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                }
                            }

                            RowLayout {
                                spacing: 6
                                Layout.fillWidth: true
                                Layout.maximumHeight: 30

                                Layout.leftMargin: 20

                                HoverableButtonTextItem {
                                    id: btnDeletAccount

                                    backgroundColor: "red"
                                    onEnterColor: Qt.rgba(150 / 256, 0, 0, 0.7)
                                    onDisabledBackgroundColor: Qt.rgba(
                                                                   255 / 256,
                                                                   0, 0, 0.8)
                                    onPressColor: backgroundColor
                                    textColor: "white"

                                    Layout.maximumWidth: 261
                                    Layout.preferredWidth: 261
                                    Layout.minimumWidth: 261

                                    Layout.minimumHeight: 30
                                    Layout.preferredHeight: 30
                                    Layout.maximumHeight: 30

                                    text: qsTr("Delete Account")
                                    font.pointSize: 10
                                    font.kerning: true

                                    radius: height / 2

                                    onClicked: {
                                        delAccountSlot()
                                    }
                                }

                                Item {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
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

                    ColumnLayout {
                        Layout.fillWidth: true

                        Label {
                            Layout.fillWidth: true

                            Layout.maximumHeight: 27
                            Layout.preferredHeight: 27
                            Layout.minimumHeight: 27

                            text: qsTr("Linked Device")
                            font.pointSize: 13
                            font.kerning: true

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item {
                            Layout.fillHeight: true

                            Layout.maximumWidth: 10
                            Layout.preferredWidth: 10
                            Layout.minimumWidth: 10
                        }

                        ColumnLayout {
                            spacing: 7

                            Layout.fillWidth: true

                            ListView {
                                id: settingsListView
                                layer.mipmap: false
                                clip: false
                                visible: true

                                Layout.leftMargin: 20

                                Layout.fillWidth: true

                                Layout.minimumHeight: 164
                                Layout.preferredHeight: 164
                                Layout.maximumHeight: 164
                            }

                            HoverableButton {
                                id: linkDevPushButton

                                visible: SettingsAdaptor.getAccountConfig_Manageruri() === ""

                                Layout.fillWidth: true

                                Layout.maximumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.minimumHeight: 30

                                radius: height / 2

                                text: qsTr("+Link Another Device")
                                font.pointSize: 10
                                font.kerning: true

                                onClicked: {
                                    showLinkDevSlot()
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

                    // banned list view
                    ColumnLayout {
                        id: bannedContactsLayoutWidget

                        Layout.fillWidth: true
                        spacing: 6

                        RowLayout {
                            Layout.leftMargin: 9
                            Layout.rightMargin: 8
                            Layout.topMargin: 1

                            Layout.fillWidth: true
                            Layout.maximumHeight: 30

                            Label {
                                Layout.preferredWidth: 164
                                Layout.minimumWidth: 164

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                text: qsTr("Banned Contact")
                                font.pointSize: 13
                                font.kerning: true

                                horizontalAlignment: Text.AlignLeft
                                verticalAlignment: Text.AlignVCenter
                            }

                            Item {
                                Layout.fillHeight: true

                                Layout.maximumWidth: 10
                                Layout.preferredWidth: 10
                                Layout.minimumWidth: 10
                            }

                            HoverableButton {
                                id: bannedContactsBtn

                                Layout.maximumWidth: 30
                                Layout.preferredWidth: 30
                                Layout.minimumWidth: 30

                                Layout.minimumHeight: 30
                                Layout.preferredHeight: 30
                                Layout.maximumHeight: 30

                                radius: height / 2

                                icon.source: "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                icon.height: 32
                                icon.width: 32
                            }

                            Item {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                            }
                        }

                        ColumnLayout {
                            spacing: 6

                            Layout.leftMargin: 9
                            Layout.rightMargin: 8
                            Layout.bottomMargin: 9
                            Item {
                                Layout.fillWidth: true

                                Layout.minimumHeight: 10
                                Layout.preferredHeight: 10
                                Layout.maximumHeight: 10
                            }

                            ListView {
                                id: bannedListWidget

                                Layout.leftMargin: 20
                                Layout.fillWidth: true
                                Layout.minimumHeight: 150
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
                    }

                    RowLayout {
                        Layout.fillWidth: true

                        Layout.minimumHeight: 30
                        Layout.preferredHeight: 30
                        Layout.maximumHeight: 30

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        HoverableButton {
                            id: advancedAccountSettingsPButton

                            Layout.minimumWidth: 180

                            Layout.minimumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.maximumHeight: 30

                            radius: height / 2

                            text: qsTr("Advanced Account Settings")
                            font.pointSize: 10
                            font.kerning: true

                            icon.source: {
                                if (advanceSettingsView.visible) {
                                    return "qrc:/images/icons/round-arrow_drop_up-24px.svg"
                                } else {
                                    return "qrc:/images/icons/round-arrow_drop_down-24px.svg"
                                }
                            }

                            icon.height: 24
                            icon.width: 24

                            onClicked: {
                                advanceSettingsView.visible = !advanceSettingsView.visible
                                if (advanceSettingsView.visible) {
                                    advanceSettingsView.updateAccountInfoDisplayedAdvance()
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true

                    Layout.minimumHeight: 48
                    Layout.preferredHeight: 48
                    Layout.maximumHeight: 48
                }

                ColumnLayout {
                    spacing: 6
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Layout.leftMargin: 30

                    // instantiate advance setting page
                    AdvancedSettingsView {
                        id: advanceSettingsView

                        Layout.leftMargin: 10
                        visible: false
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
