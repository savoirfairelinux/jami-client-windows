import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import net.jami.tools.utils 1.0

/*
 * Every QML object can be assigned an id and an objectName that other objects can use to refer to the object.
 * The difference between the two is that the id is for referencing the object within QML,
 * while the objectName is required for referencing the object from C++
 */
ColumnLayout {
    id: mainColumnLayout

    property int minWidth: 560
    property int minHeight: 600
    property int textFontSize: 10

    property string userImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string qrImageSource: "qrc:/qtquickplugin/images/template_image.png"
    property string userNameText: "Text"
    property string registeredNameText: "Text"
    property string uriText: "Text"

    spacing: 2

    Rectangle {
        id: profileRectangle

        Layout.alignment: Qt.AlignCenter
        Layout.minimumWidth: minWidth
        Layout.minimumHeight: minHeight
        Layout.maximumWidth: minWidth * 2
        Layout.maximumHeight: minHeight * 2

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

        UtilsAdapter{
            id: utilsAdapter
        }

        ColumnLayout {
            id: profileRectColumnLayout

            anchors.fill: parent
            anchors.centerIn: parent

            Rectangle {
                id: userRectangle

                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.minimumWidth: 500
                Layout.minimumHeight: 200
                Layout.maximumHeight: 1000
                Layout.maximumWidth: 1000
                color: "#ffffff"

                RowLayout {
                    id: userRectRowLayout

                    anchors.fill: parent

                    Image {
                        id: userImage

                        anchors.verticalCenter: userRectangle.verticalCenter
                        anchors.left: userRectangle.left
                        Layout.fillWidth: true
                        Layout.fillHeight :true
                        Layout.maximumWidth: 150
                        Layout.maximumHeight: 150

                        fillMode: Image.PreserveAspectFit
                        source: userImageSource
                        cache: false
                    }

                    Text {
                        id: userName

                        anchors.verticalCenter: userRectangle.verticalCenter
                        anchors.left: userImage.right
                        anchors.leftMargin: 25
                        Layout.fillWidth: true
                        Layout.fillHeight :true
                        Layout.maximumWidth: 300
                        Layout.maximumHeight: t_metrics_userName.tightBoundingRect.height

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
            }

            Rectangle {
                id: informationRectangle

                anchors.top: userRectangle.bottom
                anchors.topMargin: 10
                Layout.minimumWidth: 500
                Layout.minimumHeight: 300
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                color: "#ffffff"

                Text {
                    id: inforElement

                    anchors.left: informationRectangle.left
                    anchors.top: informationRectangle.top
                    anchors.topMargin: 30
                    anchors.leftMargin: 20
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

                ColumnLayout {
                    id: infoRectColumnLayout

                    anchors.top: informationRectangle.top
                    anchors.topMargin: 70
                    anchors.left: informationRectangle.left
                    anchors.leftMargin: 60

                    Rectangle {
                        id: subInfoRect

                        Layout.minimumWidth: 400
                        Layout.minimumHeight: 200
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                        RowLayout {
                            id: registerNameRowLayout

                            Rectangle {
                                id: registerNameRect

                                Layout.minimumWidth: subInfoRect.width
                                Layout.minimumHeight: Math.max(registeredNameElement.height, registeredName.height)

                                Label {
                                    id: registeredNameElement

                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "Username"
                                    fontSizeMode: Text.Fit
                                    font.pointSize: textFontSize
                                    minimumPointSize: textFontSize - 2
                                    color: "#828282"

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.IBeamCursor
                                        acceptedButtons: Qt.NoButton
                                    }
                                    background: Rectangle {
                                        border.width: 0
                                    }
                                }

                                TextArea  {
                                    id: registeredName

                                    anchors.left: registeredNameElement.right
                                    anchors.leftMargin: 10

                                    text: registeredNameText
                                    verticalAlignment: Text.AlignVCenter
                                    font.pointSize: textFontSize
                                    color: "black"
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
                                                    onReleased: { registeredNameContextMenuBackRect.color = "#e0e0e0"; utilsAdapter.setText(registeredName.selectedText); reset();}
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
                                        color: "transparent"
                                    }
                                }
                            }
                        }
                        RowLayout {
                            id: idRowLayout

                            anchors.top: registerNameRowLayout.bottom

                            Rectangle {
                                id: idRect

                                Layout.minimumWidth: subInfoRect.width
                                Layout.minimumHeight: Math.max(idElement.height, idText.height)

                                Label {
                                    id: idElement

                                    anchors.left: parent.left
                                    anchors.leftMargin: registeredNameElement.width - idElement.width
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "ID"
                                    fontSizeMode: Text.Fit
                                    font.pointSize: textFontSize
                                    minimumPointSize: textFontSize - 2
                                    color: "#828282"

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
                                    id: idText

                                    anchors.left: idElement.right
                                    anchors.leftMargin: 10
                                    text: uriText
                                    verticalAlignment: Text.AlignVCenter
                                    font.pointSize: textFontSize
                                    color: "black"

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
                                                    onReleased: { idBackRect.color = "#e0e0e0"; utilsAdapter.setText(idText.selectedText); reset();}
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
                        }
                        RowLayout {
                            id: qrRowLayout

                            anchors.top: idRowLayout.bottom
                            anchors.topMargin: 6

                            Rectangle {
                                id: qrRect

                                Layout.minimumWidth: subInfoRect.width
                                Layout.minimumHeight: qrElement.height

                                Label  {
                                    id: qrElement

                                    anchors.left: parent.left
                                    anchors.leftMargin: registeredNameElement.width - qrElement.width
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "QR Code"
                                    fontSizeMode: Text.Fit
                                    font.pointSize: textFontSize
                                    minimumPointSize: textFontSize - 2
                                    color: "#828282"

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.IBeamCursor
                                        acceptedButtons: Qt.NoButton
                                    }
                                    background: Rectangle {
                                        border.width: 0
                                    }
                                }

                                Image {
                                    id: qrImage

                                    anchors.left: qrElement.right
                                    anchors.leftMargin: 20
                                    fillMode: Image.PreserveAspectFit
                                    source: qrImageSource
                                    sourceSize.width: 150
                                    sourceSize.height: 150
                                    cache: false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
