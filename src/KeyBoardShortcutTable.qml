import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Rectangle {
    id: rectangle

    property int minWidth: 670
    property int minHeight: 600

    implicitWidth: minWidth;
    implicitHeight: minHeight;
    color: "white";
    radius: 30;

    Rectangle {
        width: t_metrics.tightBoundingRect.width + 10;
        height: 29
        color: "#525252";
        radius: 8;
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter;
        Text {
            id : shortcutText;
            anchors.centerIn: parent;
            anchors.leftMargin: 10;
            font.family: "Arial";
            font.pixelSize: 20
            text: "Shortcuts";
            color: "white"
        }
        TextMetrics {
            id:     t_metrics;
            font:   shortcutText.font;
            text:   shortcutText.text;
        }
    }

    TabView {
        id: tabView;
        anchors.rightMargin: 20
        anchors.leftMargin: 20
        anchors.bottomMargin: 20
        anchors.topMargin: 50

        implicitWidth: minWidth;
        implicitHeight: minHeight - 100;
        anchors.fill: parent;

        ListModel {
            id: keyboardGeneralShortcutsModel;
            ListElement {
                Shortcut: "Ctrl + J";
                Description: "Open account list";
            }
            ListElement {
                Shortcut: "Ctrl + L";
                Description: "Focus on smart list";
            }
            ListElement {
                Shortcut: "Ctrl + Up";
                Description: "Focus on the previous element on smart list";
            }
            ListElement {
                Shortcut: "Ctrl + Down";
                Description: "Focus on the next element on smart list";
            }
            ListElement {
                Shortcut: "Ctrl + F";
                Description: "Focus on contact search bar";
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
            ListElement {
                Shortcut: "Ctrl + Y";
                Description: "Answer an incoming call";
            }
            ListElement {
                Shortcut: "Ctrl + D";
                Description: "Decline the call";
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
                        font.pixelSize: 20
                        text: styleData.value;
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
        Tab {
            title: "General";
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
                        height: 20;
                        width: 0;
                        color: "white"
                    }
                }
            }
        }
        Tab {
            title: "Conversations";
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
                    frame: Rectangle {
                        border{
                            color: "transparent"; // color of the border
                        }
                    }
                    headerDelegate: Rectangle {
                        height: 20;
                        width: 0;
                        color: "white"
                    }
                }
            }
        }
        Tab {
            title: "Settings";
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
                    frame: Rectangle {
                        border{
                            color: "transparent"; // color of the border
                        }
                    }
                    headerDelegate: Rectangle {
                        height: 20;
                        width: 0;
                        color: "white"
                    }
                }
            }
        }
        style: TabViewStyle {
            tab:Rectangle
            {
                //draw half rounded rect
                id: root;
                color: "transparent";
                implicitWidth: tabView.width / 3 + 1;
                implicitHeight: 30;
                radius: 10;
                clip: true;
                Rectangle {
                    id: clipped;
                    width: parent.width;
                    height: parent.height;
                    radius: root.radius;
                    color: "#f0f0f0";
                    border.color: "white";
                }
                Text {
                    id: tabText;
                    anchors.centerIn: root;
                    text: styleData.title;
                    color: "#202020";
                }
                MouseArea {
                    anchors.fill: parent;
                    function tabIndex() {
                        switch(styleData.title) {
                          case "General":
                            return 0;
                          case "Conversations":
                            return 1;
                          case "Settings":
                            return 2;
                          default:
                            return -1;
                        }
                    }
                    hoverEnabled: true;
                    onPressed: { clipped.color = "#c0c0c0"; tabView.currentIndex = tabIndex(); }
                    onReleased: { clipped.color = "#f0f0f0"; }
                    onEntered: { clipped.color = "#f9f9f9"; }
                    onExited: { clipped.color = "#f0f0f0"; }
                }
            }
            tabBar: Rectangle { color: "white"; anchors.fill: parent; }
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:29}
}
##^##*/
