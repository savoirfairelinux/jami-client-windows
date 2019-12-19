import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3

Rectangle {
    id: rectangle

    property int minWidth: 1002
    property int minHeight: 500

    implicitWidth: minWidth
    implicitHeight: minHeight
    color: "white"
    radius: 30

    Rectangle {
        width: 500
        height: t_metrics_title.tightBoundingRect.height + 15
        color: "#e0e0e0"
        radius: 8
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        Text {
            id : titleText
            anchors.centerIn: parent
            anchors.leftMargin: 10
            font.family: "Arial"
            font.pixelSize: 20
            font.bold: true
            text: "Shortcuts"
            color: "black"
        }
        TextMetrics {
            id:     t_metrics_title
            font:   titleText.font
            text:   titleText.text
        }
    }

    ListModel {
        id: keyboardGeneralShortcutsModel;
        ListElement {
            Shortcut: "Ctrl + J";
            Description: "Open account list";
        }
        ListElement {
            Shortcut: "Ctrl + L";
            Description: "Focus on the smart list";
        }
        ListElement {
            Shortcut: "Ctrl + Up";
            Description: "Focus the previous element on smart list";
        }
        ListElement {
            Shortcut: "Ctrl + Down";
            Description: "Focus the next element on smart list";
        }
        ListElement {
            Shortcut: "Ctrl + F";
            Description: "Focus on contact search bar";
        }
        ListElement {
            Shortcut: "F10";
            Description: "Open keyboard shortcuts table";
        }
        ListElement {
            Shortcut: "F11";
            Description: "Maximize Screen";
        }
    }
    ListModel {
        id: keyboardConversationShortcutsModel;
        ListElement {
            Shortcut: "Shift + Ctrl + C";
            Description: "Start an audio call";
        }
        ListElement {
            Shortcut: "Shift + Ctrl + X";
            Description: "Start an video call";
        }
        ListElement {
            Shortcut: "Shift + Ctrl + L";
            Description: "Clear history";
        }
        ListElement {
            Shortcut: "Shift + Ctrl + B";
            Description: "Block contact";
        }
        ListElement {
            Shortcut: "Shift + Ctrl + J";
            Description: "Copy contact name";
        }
    }
    ListModel {
        id: keyboardSettingsShortcutsModel;
        ListElement {
            Shortcut: "Ctrl + M";
            Description: "Toggle media settings";
        }
        ListElement {
            Shortcut: "Ctrl + G";
            Description: "Toggle general settings";
        }
        ListElement {
            Shortcut: "Ctrl + I";
            Description: "Toggle account settings";
        }
    }
    ListModel {
        id: keyboardCallsShortcutsModel;
        ListElement {
            Shortcut: "Ctrl + Y";
            Description: "Answer an incoming call";
        }
        ListElement {
            Shortcut: "Ctrl + D";
            Description: "Decline the call";
        }
    }
    Component {
        id: shortcutDelegate;

        Rectangle {
            implicitWidth: minWidth /2;
            implicitHeight: 50;
            anchors.left: parent.left;
            anchors.leftMargin: 50;
            color: "white";
            border.color: "white";
            Rectangle{
                width: t_metrics.tightBoundingRect.width + 10;
                height: t_metrics.tightBoundingRect.height + 10;
                color: "#525252";
                radius: 5;
                anchors.centerIn: parent;
                Text {
                    id : shortcutText;
                    anchors.centerIn: parent;
                    anchors.leftMargin: 10;
                    font.family: "Arial";
                    font.pixelSize: 20;
                    text: styleData.value;
                    //text: Qt.binding(function(){return styleData.value;});
                    color: "white"
                }
                TextMetrics {
                    id:     t_metrics;
                    font:   shortcutText.font;
                    text:   shortcutText.text;
                }
            }
        }
    }
    Component {
        id: descriptionDelegate;

        Rectangle {
            implicitWidth: minWidth /2;
            implicitHeight: 50;

            color: "white";
            border.color: "white";
            Text {
                id : descriptionText;
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10;
                font.family: "Arial";
                font.pixelSize: 15;
                text: styleData.value;
            }
        }
    }

    Column {
        spacing: 2
        id: columnAll;
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.bottomMargin: 20
        anchors.topMargin: 50

        width: minWidth
        height: minHeight - 100
        anchors.fill: parent

        StackLayout {
            implicitWidth: parent.width
            implicitHeight: parent.height - tabBar.height
            currentIndex: tabBar.currentIndex
            Item {
                id: tabOne
                Rectangle {
                    implicitWidth: parent.width / 2
                    implicitHeight: parent.height
                    anchors.left: parent.left
                    TableView {
                        id: generalTableView;
                        anchors.fill: parent;
                        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff;
                        TableViewColumn {
                            role: "Description";
                            width: generalTableView.width / 2;
                            delegate: descriptionDelegate;
                        }
                        TableViewColumn {
                            role: "Shortcut";
                            width: generalTableView.width / 2;
                            delegate: shortcutDelegate;
                        }
                        model: keyboardGeneralShortcutsModel;
                        rowDelegate: Rectangle {
                            height: 50;
                            color: "white"
                         }
                        style: TableViewStyle {
                            alternateBackgroundColor: "white";
                            frame: Rectangle {
                                border{
                                    color: "transparent"; // color of the border
                                }
                            }
                            headerDelegate: Rectangle {
                                // Only first column's header is shown
                                height: [t_metrics_general.tightBoundingRect.height + 10, 0][styleData.column % 2]
                                width: [parent.width, 0][styleData.column % 2]
                                color: "#e0e0e0"
                                radius: 10
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                Text {
                                    id : generalShortcutText
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.family: "Arial"
                                    font.pixelSize: 20
                                    text: ["General", ""][styleData.column % 2]
                                    color: "black"
                                }
                                TextMetrics {
                                    id:     t_metrics_general
                                    font:   generalShortcutText.font
                                    text:   generalShortcutText.text
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    implicitWidth: parent.width / 2
                    implicitHeight: parent.height
                    anchors.right: parent.right

                    TableView {
                        id: conversationsTableView;
                        anchors.fill: parent;
                        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff;
                        TableViewColumn {
                            role: "Description";
                            width: conversationsTableView.width / 2;
                            delegate: descriptionDelegate;
                        }
                        TableViewColumn {
                            role: "Shortcut";
                            width: conversationsTableView.width / 2;
                            delegate: shortcutDelegate;
                        }
                        model: keyboardConversationShortcutsModel;
                        rowDelegate: Rectangle {
                            height: 50;
                            color: "white"
                         }
                        style: TableViewStyle {
                            alternateBackgroundColor: "white";
                            frame: Rectangle {
                                border{
                                    color: "transparent"; // color of the border
                                }
                            }
                            headerDelegate: Rectangle {
                                // Only first column's header is shown
                                height: [t_metrics_conversations.tightBoundingRect.height + 10, 0][styleData.column % 2]
                                width: [parent.width, 0][styleData.column % 2]
                                color: "#e0e0e0"
                                radius: 10
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                Text {
                                    id : conversationsShortcutText
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.family: "Arial"
                                    font.pixelSize: 20
                                    text: ["Conversations", ""][styleData.column % 2]
                                    color: "black"
                                }
                                TextMetrics {
                                    id:     t_metrics_conversations
                                    font:   conversationsShortcutText.font
                                    text:   conversationsShortcutText.text
                                }
                            }
                        }
                    }
                }
            }
            Item {
                id: tabTwo
                Rectangle {
                    implicitWidth: parent.width / 2
                    implicitHeight: parent.height
                    anchors.left: parent.left
                    TableView {
                        id: callsTableView;
                        anchors.fill: parent;
                        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff;
                        TableViewColumn {
                            role: "Description";
                            width: callsTableView.width / 2;
                            delegate: descriptionDelegate;
                        }
                        TableViewColumn {
                            role: "Shortcut";
                            width: callsTableView.width / 2;
                            delegate: shortcutDelegate;
                        }
                        model: keyboardCallsShortcutsModel;
                        rowDelegate: Rectangle {
                            height: 50;
                            color: "white"
                         }
                        style: TableViewStyle {
                            alternateBackgroundColor: "white";
                            frame: Rectangle {
                                border{
                                    color: "transparent"; // color of the border
                                }
                            }
                            headerDelegate: Rectangle {
                                // Only first column's header is shown
                                height: [t_metrics_calls.tightBoundingRect.height + 10, 0][styleData.column % 2]
                                width: [parent.width, 0][styleData.column % 2]
                                color: "#e0e0e0"
                                radius: 10
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                Text {
                                    id : callsShortcutText
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.family: "Arial"
                                    font.pixelSize: 20
                                    text: parent.width ? "Calls" : ""
                                    color: "black"
                                }
                                TextMetrics {
                                    id:     t_metrics_calls
                                    font:   callsShortcutText.font
                                    text:   callsShortcutText.text
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    implicitWidth: parent.width / 2
                    implicitHeight: parent.height
                    anchors.right: parent.right
                    TableView {
                        id: settingsTableView;
                        anchors.fill: parent;
                        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff;
                        TableViewColumn {
                            role: "Description";
                            width: settingsTableView.width / 2;
                            delegate: descriptionDelegate;
                        }
                        TableViewColumn {
                            role: "Shortcut";
                            width: settingsTableView.width / 2;
                            delegate: shortcutDelegate;
                        }
                        model: keyboardSettingsShortcutsModel;
                        rowDelegate: Rectangle {
                            height: 50;
                            color: "white"
                         }
                        style: TableViewStyle {
                            alternateBackgroundColor: "white";
                            frame: Rectangle {
                                border{
                                    color: "transparent"; // color of the border
                                }
                            }
                            headerDelegate: Rectangle {
                                // Only first column's header is shown
                                height: [t_metrics_settings.tightBoundingRect.height + 10, 0][styleData.column % 2]
                                width: [parent.width, 0][styleData.column % 2]
                                color: "#e0e0e0"
                                radius: 10
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                Text {
                                    id : settingsShortcutText
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    font.family: "Arial"
                                    font.pixelSize: 20
                                    text: parent.width ? "Settings" : ""
                                    color: "black"
                                }
                                TextMetrics {
                                    id:     t_metrics_settings
                                    font:   settingsShortcutText.font
                                    text:   settingsShortcutText.text
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TabBar {
        id: tabBar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        width: 240
        currentIndex: 0
        TabButton {
            id: pageOne
            width: tabBar.width / 2
            text: qsTr("1")
            down: true
            contentItem: Text {
                text: pageOne.text
                font: pageOne.font
                opacity: enabled ? 1.0 : 0.3
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                id: buttonRectOne
                implicitWidth: tabBar.width / 2
                implicitHeight: tabBar.height
                radius: 10
                color: pageOne.down ? "#e0e0e0" :"#fdfdfd"
                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onPressed: { buttonRectOne.color = "#c0c0c0"; tabBar.currentIndex = 0; pageOne.down = true; pageTwo.down = false;}
                    onReleased: { buttonRectOne.color = "#e0e0e0"; }
                    onEntered: { buttonRectOne.color = "#c7c7c7"; }
                    onExited: { buttonRectOne.color = Qt.binding(function() { return pageOne.down ? "#e0e0e0" :"#fdfdfd" }); }
                }
            }
        }
        TabButton {
            id: pageTwo
            text: qsTr("2")
            width: tabBar.width / 2
            contentItem: Text {
                text: pageTwo.text
                font: pageTwo.font
                opacity: enabled ? 1.0 : 0.3
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                id: buttonRectTwo
                implicitWidth: tabBar.width / 2
                implicitHeight: tabBar.height
                radius: 10
                color: pageTwo.down ? "#e0e0e0" :"#fdfdfd"
                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onPressed: { buttonRectTwo.color = "#c0c0c0"; tabBar.currentIndex = 1; pageTwo.down = true; pageOne.down = false;}
                    onReleased: { buttonRectTwo.color = "#e0e0e0"; }
                    onEntered: { buttonRectTwo.color = "#c7c7c7"; }
                    onExited: { buttonRectTwo.color = Qt.binding(function() { return pageTwo.down ? "#e0e0e0" :"#fdfdfd" }); }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:29}
}
##^##*/
