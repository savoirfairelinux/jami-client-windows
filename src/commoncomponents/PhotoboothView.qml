import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Styles 1.4
import net.jami.PhotoboothPreviewRender 1.0

ColumnLayout{
    enum ForegroundMode{
        PHOTOBOOTH,
        AVATAR
    }

    property int boothWidht: 224
    property int forgroundMode: PhotoboothView.AVATAR

    spacing: 0

    Layout.maximumWidth: boothWidht
    Layout.preferredWidth: boothWidht
    Layout.minimumWidth: boothWidht

    Layout.maximumHeight: 0

    Label{
        id: avatarLabel

        visible: forgroundMode === PhotoboothView.PHOTOBOOTH? true : false

        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht
    }

    PhotoboothPreviewRender{
        id:photoboothPreviewRender

        visible: forgroundMode === PhotoboothView.PHOTOBOOTH? true : false

        Layout.maximumWidth: boothWidht
        Layout.preferredWidth: boothWidht
        Layout.minimumWidth: boothWidht

        Layout.maximumHeight: boothWidht
        Layout.preferredHeight: boothWidht
        Layout.minimumHeight: boothWidht
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
            source: "qrc:/images/icons/baseline-camera_alt-24px.svg"

            onClicked: {}
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

            onClicked: {}
        }

        Item{
            Layout.fillHeight: true

            Layout.maximumWidth: 40
            Layout.preferredWidth: 40
            Layout.minimumWidth: 40
        }
    }
}
