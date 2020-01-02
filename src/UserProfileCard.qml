import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

/*
 * Every QML object can be assigned an id and an objectName that other objects can use to refer to the object.
 * The difference between the two is that the id is for referencing the object within QML,
 * while the objectName is required for referencing the object from C++
 */

Rectangle {
    id: profileRectangle

    property int minWidth: 650
    property int minHeight: 600
    property string userImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string qrImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string userNameText: "Text"
    property string registeredNameText: "Text"
    property string uriText: "Text"

    implicitWidth: minWidth
    implicitHeight: minHeight
    color: "red"
    radius: 30

    Rectangle {
        id: userRectangle

        anchors.horizontalCenter: profileRectangle.horizontalCenter
        anchors.top: profileRectangle.top
        anchors.topMargin: 20
        width: 500
        height: 200
        color: "#ffffff"

        Image {
            id: userImage

            anchors.verticalCenter: userRectangle.verticalCenter
            anchors.left: userRectangle.left
            width: 150
            height: 150
            fillMode: Image.PreserveAspectFit
            source: userImageSource
        }

        Text {
            id: userName

            anchors.verticalCenter: userRectangle.verticalCenter
            anchors.left: userImage.right
            anchors.leftMargin: 25
            width: t_metrics_userName.tightBoundingRect.width
            height: t_metrics_userName.tightBoundingRect.width
            text: userNameText
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 30
            TextMetrics {
                id:     t_metrics_userName
                font:   userName.font
                text:   userName.text
            }
        }
    }

    Rectangle {
        id: informationRectangle

        anchors.horizontalCenter: profileRectangle.horizontalCenter
        anchors.bottom: profileRectangle.bottom
        anchors.bottomMargin: 20
        anchors.top: userRectangle.bottom
        anchors.topMargin: 30
        width: 500
        height: 200
        color: "#ffffff"

        Text {
            id: inforElement

            anchors.left: informationRectangle.left
            anchors.top: informationRectangle.top
            width: t_metrics_info.tightBoundingRect.width
            height: t_metrics_info.tightBoundingRect.height
            text: qsTr("Information")
            font.pixelSize: 20
            TextMetrics {
                id:     t_metrics_info
                font:   inforElement.font
                text:   inforElement.text
            }
        }

        Rectangle{
            id: elementsRect

            anchors.left: informationRectangle.left
            anchors.top: inforElement.bottom
            anchors.topMargin: 20

            width: 150
            height: 150

            TextField {
                id: registeredNameElement

                anchors.right: elementsRect.right
                anchors.top: elementsRect.top
                anchors.topMargin: 5
                width: t_metrics_registeredNameElement.tightBoundingRect.width + 20
                height: t_metrics_registeredNameElement.tightBoundingRect.height + 15
                text: "Username "
                font.pixelSize: 18
                textColor: "#828282"
                TextMetrics {
                    id:     t_metrics_registeredNameElement
                    font:   registeredNameElement.font
                    text:   registeredNameElement.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
                style: TextFieldStyle{
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }

            TextField {
                id: idElement

                anchors.right: elementsRect.right
                anchors.top: registeredNameElement.bottom
                anchors.topMargin: 5
                width: t_metrics_idElement.tightBoundingRect.width + 20
                height: t_metrics_idElement.tightBoundingRect.height + 15
                text: "ID "
                font.pixelSize: 18
                textColor: "#828282"
                TextMetrics {
                    id:     t_metrics_idElement
                    font:   idElement.font
                    text:   idElement.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
                style: TextFieldStyle{
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }

            TextField {
                id: qrElement

                anchors.right: elementsRect.right
                anchors.top: idElement.bottom
                anchors.topMargin: 5
                width: t_metrics_qrElement.tightBoundingRect.width + 20
                height: t_metrics_qrElement.tightBoundingRect.height + 15
                text: "QR Code "
                font.pixelSize: 18
                textColor: "#828282"
                TextMetrics {
                    id:     t_metrics_qrElement
                    font:   qrElement.font
                    text:   qrElement.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
                style: TextFieldStyle{
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }
        }

        Rectangle{
            id: contentsElementsRect

            anchors.left: elementsRect.right
            anchors.leftMargin: 1
            anchors.top: inforElement.bottom
            anchors.topMargin: 20

            width: 150
            height: 70

            TextField {
                id: registeredName

                anchors.left: contentsElementsRect.left
                anchors.leftMargin: 2
                anchors.top: contentsElementsRect.top
                anchors.topMargin: 7
                width: t_metrics_registeredName.tightBoundingRect.width + 20
                height: t_metrics_registeredName.tightBoundingRect.height + 15
                text: registeredNameText
                font.pixelSize: 18
                textColor: "black"
                TextMetrics {
                    id:     t_metrics_registeredName
                    font:   registeredName.font
                    text:   registeredName.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
                style: TextFieldStyle{
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }

            TextField {
                id: idText

                anchors.left: contentsElementsRect.left
                anchors.leftMargin: 2
                anchors.top: registeredName.bottom
                anchors.topMargin: 3
                width: t_metrics_idText.tightBoundingRect.width + 20
                height: t_metrics_idText.tightBoundingRect.height + 15
                text: uriText
                font.pixelSize: 18
                textColor: "black"
                TextMetrics {
                    id:     t_metrics_idText
                    font:   idText.font
                    text:   idText.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                }
                style: TextFieldStyle{
                    background: Rectangle {
                        border.width: 0
                    }
                }
            }
        }
        Image {
            id: qrImage

            anchors.top: contentsElementsRect.bottom
            anchors.topMargin: 6
            anchors.left: elementsRect.right
            anchors.leftMargin: 1
            fillMode: Image.PreserveAspectFit
            source: qrImageSource
            sourceSize.width: 200
            sourceSize.height: 200
        }
    }
}
