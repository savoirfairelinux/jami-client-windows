import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0

Dialog {
    id: userQrImageDialog

    property int currentAccountIndex: 0

    // when dialog is opened, trigger mainViewWindow overlay which is defined in overlay.model (model : true is necessary)
    modal: true

    // content height + margin
    contentHeight: userQrImage.height + 10

    Image {
        id: userQrImage

        anchors.centerIn: parent

        width: 300
        height: 300

        fillMode: Image.PreserveAspectFit
        source: "image://qrImage/account_" + currentAccountIndex + "_0_qr"
        // requestedSize
        sourceSize.width: 310
        sourceSize.height: 310
        mipmap: true
    }

    background: Rectangle {
        border.width: 0
        radius: 10
    }
}
