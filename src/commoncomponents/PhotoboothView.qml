import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.0
import net.jami.UtilsAdapter 1.0
import net.jami.PhotoboothPreviewRender 1.0

ColumnLayout{
    property bool takePhotoState: false
    property bool hasAvatar: false
    property bool isDefaultIcon: false
    property string fileName: ""
    property string imgBase64: ""

    property int boothWidht: 224

    signal imageAcquired
    signal imageCleared

    function startBooth(force = false){
        hasAvatar = false
        UtilsAdapter.startPreviewing(force)
        takePhotoState = true
    }

    function stopBooth(){
        if(!UtilsAdapter.hasVideoCall()) {
            UtilsAdapter.stopPreviewing()
        }

        takePhotoState = false
    }

    function setAvatarPixmap(avatarPixmapBase64, defaultValue = false){
        imgBase64 = avatarPixmapBase64
        stopBooth()
        if(defaultValue){
            isDefaultIcon = defaultValue
        }
    }

    onVisibleChanged: {
        if(!visible){
            stopBooth()
        }
    }

    FileDialog{
        id: importFromFileToAvatar_Dialog

        selectExisting: true
        selectMultiple: false
        title: qsTr("Choose an image to be the avatar")
        folder: shortcuts.pictures

        nameFilters: [ qsTr("Image Files") + " (*.jpg *.jpeg *.png)",qsTr(
                "All files") + " (*)"]

        onAccepted: {
            fileName = fileUrl
            if(fileName.length == 0) {
                imageCleared()
                return
            }
            imgBase64 = UtilsAdapter.getCroppedImageBase64FromFile(fileName.replace("file:///", ""),boothWidht)
            imageAcquired()
            stopBooth()
        }
    }

    spacing: 0

    Layout.maximumWidth: boothWidht
    Layout.preferredWidth: boothWidht
    Layout.minimumWidth: boothWidht

    Layout.maximumHeight: 0

    Layout.alignment: Qt.AlignHCenter

    Label{
        id: avatarLabel

        visible: !takePhotoState

        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht

        Layout.alignment: Qt.AlignHCenter

        background: Rectangle {
            id: avatarLabelBackground

            anchors.fill: parent
            color: "transparent"

            Image{
                id: avatarImg

                anchors.fill: parent
                source: "data:image/png;base64," + imgBase64
                fillMode: Image.PreserveAspectCrop
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Rectangle{
                        width: avatarImg.width
                        height: avatarImg.height
                        radius: {
                            var size = ((avatarImg.width <= avatarImg.height)? avatarImg.width:avatarImg.height)
                            return size /2
                        }
                    }
                }
            }
        }
    }

    PhotoboothPreviewRender{
        id:previewWidget

        onHideBooth:{
            stopBooth()
        }
        visible: takePhotoState
        focus: visible

        Layout.alignment: Qt.AlignHCenter
        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht

        layer.enabled: true
        layer.effect: OpacityMask {
            maskSource: Rectangle{
                width: previewWidget.width
                height: previewWidget.height
                radius: {
                    var size = ((previewWidget.width <= previewWidget.height)? previewWidget.width:previewWidget.height)
                    return size /2
                }
            }
        }

        Label{
            id: flashOverlay

            anchors.fill: previewWidget
            visible: false
            color: "#fff"

            OpacityAnimator on opacity{
                id: flashAnimation

                easing: Easing.OutCubic
                from: 1
                to: 0
                duration: 600
            }
        }
    }


    RowLayout{
        Layout.fillWidth: true
        Layout.minimumHeight: 30
        Layout.maximumHeight: 30

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        HoverableButton {
            id: takePhotoButton

            property string cameraAltIconUrl: "qrc:/images/icons/baseline-camera_alt-24px.svg"
            property string addPhotoIconUrl: "qrc:/images/icons/round-add_a_photo-24px.svg"
            property string refreshIconUrl: "qrc:/images/icons/baseline-refresh-24px.svg"

            Layout.maximumWidth: 30
            Layout.preferredWidth: 30
            Layout.minimumWidth: 30

            Layout.minimumHeight: 30
            Layout.preferredHeight: 30
            Layout.maximumHeight: 30

            text: ""
            font.pointSize: 10
            font.kerning: true

            radius: height / 6
            source: {
                if(isDefaultIcon){
                    return addPhotoIconUrl
                }

                if(takePhotoState) {
                    return cameraAltIconUrl
                }

                if(hasAvatar){
                    return refreshIconUrl
                } else {
                    return addPhotoIconUrl
                }
            }
            onClicked: {
                if(!takePhotoState){
                    imageCleared()
                    startBooth()
                    return
                } else {
                    // show flash overlay
                    flashOverlay.visible = true
                    flashAnimation.restart()

                    // run concurrent function call to take photo
                    imgBase64 = previewWidget.takeCroppedPhotoToBase64(boothWidht)
                    hasAvatar = true
                    imageAcquired()
                    stopBooth()
                }
            }
        }

        Item{
            Layout.fillHeight: true

            Layout.minimumWidth: 6
            Layout.preferredWidth: 6
            Layout.maximumWidth: 6
        }

        HoverableButton {
            id: importButton

            Layout.maximumWidth: 30
            Layout.preferredWidth: 30
            Layout.minimumWidth: 30

            Layout.minimumHeight: 30
            Layout.preferredHeight: 30
            Layout.maximumHeight: 30

            text: ""
            font.pointSize: 10
            font.kerning: true

            radius: height / 6
            source: "qrc:/images/icons/round-folder-24px.svg"

            onClicked: {
                importFromFileToAvatar_Dialog.open()
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
