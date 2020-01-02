import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import MyQClipboard 1.0

/*
 * Every QML object can be assigned an id and an objectName that other objects can use to refer to the object.
 * The difference between the two is that the id is for referencing the object within QML,
 * while the objectName is required for referencing the object from C++
 */

Rectangle {
    id: profileRectangle

    property int minWidth: 550
    property int minHeight: 590
    property int textFontSize: 10

    property string userImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string qrImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string userNameText: "Text"
    property string registeredNameText: "Text"
    property string uriText: "Text"

    implicitWidth: minWidth
    implicitHeight: minHeight
    color: "white"
    radius: 30

    MouseArea {
        // trick: in future qt version, use popup() instead of open()
        //        for menu item (will not auto close)
        anchors.fill: parent
        propagateComposedEvents: true
        onClicked: {
            if(Qt.platform.os == "windows") {
                registeredNameContextMenu.close()
                idContextMenu.close()
            }
        }
    }

    QClipboard{
        id: clipboard
    }

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
            cache: false
        }

        Text {
            id: userName

            anchors.verticalCenter: userRectangle.verticalCenter
            anchors.left: userImage.right
            anchors.leftMargin: 25
            width: 300
            height: t_metrics_userName.tightBoundingRect.height
            text: userNameText
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
            font.pointSize: 20
            elide: Text.ElideRight
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
        anchors.topMargin: 10
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
            font.pointSize: textFontSize
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
                anchors.topMargin: 7
                width: t_metrics_registeredNameElement.tightBoundingRect.width + 22
                height: t_metrics_registeredNameElement.tightBoundingRect.height + 20
                text: "Username "
                font.pointSize: textFontSize
                color: "#828282"
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
                background: Rectangle {
                    border.width: 0
                }
            }

            TextField {
                id: idElement

                anchors.right: elementsRect.right
                anchors.top: registeredNameElement.bottom
                anchors.topMargin: 5
                width: t_metrics_idElement.tightBoundingRect.width + 22
                height: t_metrics_idElement.tightBoundingRect.height + 20
                text: "ID "
                font.pointSize: textFontSize
                color: "#828282"
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
                background: Rectangle {
                    border.width: 0
                }
            }

            TextField {
                id: qrElement

                anchors.right: elementsRect.right
                anchors.top: idElement.bottom
                anchors.topMargin: 5
                width: t_metrics_qrElement.tightBoundingRect.width + 22
                height: t_metrics_qrElement.tightBoundingRect.height + 20
                text: "QR Code "
                font.pointSize: textFontSize
                color: "#828282"
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
                background: Rectangle {
                    border.width: 0
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

                anchors.left: registeredNameElement.right
                anchors.leftMargin: 10
                anchors.top: contentsElementsRect.top
                anchors.topMargin: 7
                width: t_metrics_registeredName.tightBoundingRect.width + 22
                height: t_metrics_registeredName.tightBoundingRect.height + 20
                text: registeredNameText
                font.pointSize: textFontSize
                color: "black"
                TextMetrics {
                    id:     t_metrics_registeredName
                    font:   registeredName.font
                    text:   registeredName.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        if (mouse.button === Qt.RightButton && Qt.platform.os == "windows") {
                            // make menu pos at mouse
                            var relativeMousePos = mapToItem(registeredName, mouse.x, mouse.y)
                            registeredNameContextMenu.x = relativeMousePos.x
                            registeredNameContextMenu.y = relativeMousePos.y
                            registeredNameContextMenu.open()
                        }
                    }
                }
                Menu {
                    id: registeredNameContextMenu
                    MenuItem {
                        id: registeredNameItem
                        text: qsTr("Copy")
                        background: Rectangle {
                            id: registeredNameContextMenuBackRect

                            implicitWidth: 150
                            implicitHeight: 30
                            border.width: 1
                            border.color: "black"
                            color: registeredNameItem.down ? "#e0e0e0" :"#fdfdfd"
                            MouseArea {
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onPressed: { registeredNameContextMenuBackRect.color = "#c0c0c0"; }
                                onReleased: { registeredNameContextMenuBackRect.color = "#e0e0e0"; clipboard.setText(registeredName.selectedText); reset();}
                                onEntered: { registeredNameContextMenuBackRect.color = "#c7c7c7"; }
                                onExited: { registeredNameContextMenuBackRect.color = Qt.binding(function() { return registeredNameItem.down ? "#e0e0e0" :"#fdfdfd" }); }
                                function reset(){
                                    registeredName.deselect()
                                    registeredNameContextMenu.close()
                                }
                            }
                        }
                    }
                    background: Rectangle {
                        implicitWidth: 150
                        implicitHeight: 30
                    }
                }
                background: Rectangle {
                    border.width: 0
                }
            }

            TextField {
                id: idText

                anchors.left: idElement.right
                anchors.leftMargin: 10
                anchors.top: registeredName.bottom
                anchors.topMargin: 5
                width: t_metrics_idText.tightBoundingRect.width + 22
                height: t_metrics_idText.tightBoundingRect.height + 20
                text: uriText
                font.pointSize: textFontSize
                color: "black"
                TextMetrics {
                    id:     t_metrics_idText
                    font:   idText.font
                    text:   idText.text
                }
                readOnly: true
                selectByMouse: true
                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        if (mouse.button === Qt.RightButton && Qt.platform.os == "windows") {
                            // make menu pos at mouse
                            var relativeMousePos = mapToItem(idText, mouse.x, mouse.y)
                            idContextMenu.x = relativeMousePos.x
                            idContextMenu.y = relativeMousePos.y
                            idContextMenu.open()
                        }
                    }
                }
                Menu {
                    id: idContextMenu
                    MenuItem {
                        id: idItem
                        text: qsTr("Copy")
                        background: Rectangle {
                            id: idBackRect

                            implicitWidth: 150
                            implicitHeight: 30
                            color: idItem.down ? "#e0e0e0" :"#fdfdfd"
                            border.width: 1
                            border.color: "black"
                            MouseArea {
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onPressed: { idBackRect.color = "#c0c0c0"; }
                                onReleased: { idBackRect.color = "#e0e0e0"; clipboard.setText(idText.selectedText); reset();}
                                onEntered: { idBackRect.color = "#c7c7c7"; }
                                onExited: { idBackRect.color = Qt.binding(function() { return idItem.down ? "#e0e0e0" :"#fdfdfd" }); }
                                function reset(){
                                    idText.deselect()
                                    idContextMenu.close()
                                }
                            }
                        }
                    }
                    background: Rectangle {
                        implicitWidth: 150
                        implicitHeight: 30
                    }
                }
                background: Rectangle {
                    border.width: 0
                }
            }
        }
        Image {
            id: qrImage

            anchors.top: contentsElementsRect.bottom
            anchors.topMargin: 25
            anchors.left: elementsRect.right
            anchors.leftMargin: 14
            fillMode: Image.PreserveAspectFit
            source: qrImageSource
            sourceSize.width: 200
            sourceSize.height: 200
            cache: false
        }
    }
}
