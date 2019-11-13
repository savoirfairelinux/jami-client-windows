import QtQuick 2.0

Item {

    Rectangle {
        id: page
        width: parent.width; height: parent.height;
        Text {
            id: name
            width: 231
            height: 120
            text: qsTr("It's QML")
            fontSizeMode: Text.FixedSize
            styleColor: "#1c1ce3"
            color: "green"
            anchors.centerIn: parent
        }
        color: "red"
    }
}
