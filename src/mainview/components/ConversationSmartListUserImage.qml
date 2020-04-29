import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.JamiTheme 1.0

Image {
    id: userImage

    width: 50
    height: 50

    fillMode: Image.PreserveAspectFit
    source: "data:image/png;base64," + Picture
    mipmap: true

    Rectangle {
        id: presenseRect

        anchors.right: userImage.right
        anchors.rightMargin: 1
        anchors.bottom: userImage.bottom
        anchors.bottomMargin: 2

        width: 14
        height: 14

        // enum REGISTERED == 5
        visible: Presence

        Rectangle {
            id: presenseCycle

            anchors.centerIn: presenseRect

            width: 10
            height: 10

            radius: 30
            color: JamiTheme.presenceGreen
        }

        radius: 30
        color: "white"
    }

    Rectangle {
        id: unreadMessageCountRect

        anchors.right: userImage.right
        anchors.rightMargin: 1
        anchors.top: userImage.top
        anchors.topMargin: 2

        width: 14
        height: 14

        visible: UnreadMessagesCount > 0

        Text {
            id: unreadMessageCounttext

            anchors.centerIn: unreadMessageCountRect

            text: UnreadMessagesCount > 9 ? "···" : UnreadMessagesCount
            color: "white"
        }

        radius: 30
        color: JamiTheme.notificationRed
    }
}
