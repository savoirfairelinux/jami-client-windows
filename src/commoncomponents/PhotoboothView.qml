import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3
import net.jami.LrcGeneralAdapter 1.0
import net.jami.PhotoboothPreviewRender 1.0

ColumnLayout{
    property bool takePhotoState: false
    property bool hasAvatar: false
    property bool defaultIcon: true
    property string fileName: ""

    property int boothWidht: 224

    signal imageAcquired
    signal imageCleared

    function startBooth(force = false){
        hasAvatar = fasle
        LrcGeneralAdapter.startPreviewing(force)
        takePhotoState = true
    }

    function stopBooth(){
        if(!LrcGeneralAdapter.hasVideoCall()) {
            LrcGeneralAdapter.stopPreviewing()
        }

        takePhotoState = false
    }

    function setAvatarPixmap(avatarPixmap, defaultValue = false){
        avatarImg = avatarPixmap
        stopBooth()
        if(defaultValue){

        }
    }

    FileDialog{
        id: importFromFileToAvatar_Dialog

        selectExisting: true
        selectMultiple: false
        title: qsTr("Choose an image to be the avatar")
        folder: shortcuts.pictures

        nameFilters: [ qsTr("Image Files") + " (*.jpg *.jpeg *.png)"]

        onAccepted: {
            fileName = fileUrl
        }
    }

    spacing: 0

    Layout.maximumWidth: boothWidht
    Layout.preferredWidth: boothWidht
    Layout.minimumWidth: boothWidht

    Layout.maximumHeight: 0

    Label{
        id: avatarLabel

        visible: !takePhotoState

        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht

        background: Rectangle {
            id: avatarLabelBackground

            anchors.fill: parent
            color: "black"

            Image{
                id: avatarImg

                property var imgBase64: ""

                anchors.fill: parent
                source: "data:image/png;base64," + imgBase64
            }
        }
    }

    PhotoboothPreviewRender{
        id:previewWidget

        visible: takePhotoState

        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht

        Overlay.overlay: Label{
            id: flashOverlay

            anchors.fill: previewWidget
            visible: false
            color: "#fff"

            onVisibleChanged:{
                if(visible){
                    flashAnimation.restart()
                } else {
                    flashAnimation.stop()
                }
            }

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
        Layout.preferedHeight: 30
        Layout.maximumHeight: 30

        Item{
            Layout.fillHeight: true

            Layout.maximumWidth: 40
            Layout.preferredWidth: 40
            Layout.minimumWidth: 40
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
                if(defaultIcon){
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

                }
            }
        }

        Item{
            Layout.fillWidth: true
            Layout.fillHeight: true
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
                if(fileName.length === 0){
                    imageCleared()
                    return
                }

            }
        }

        Item{
            Layout.fillHeight: true

            Layout.maximumWidth: 40
            Layout.preferredWidth: 40
            Layout.minimumWidth: 40
        }
    }
}
