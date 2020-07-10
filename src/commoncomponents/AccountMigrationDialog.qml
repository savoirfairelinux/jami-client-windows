/*
 * Copyright (C) 2019-2020 by Savoir-faire Linux
 * Author: Yang Wang   <yang.wang@savoirfairelinux.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import net.jami.Models 1.0

import "../constant"
import "../wizardview/components"
/*
 * Account Migration Dialog for migrating account
 */

Dialog {
    id: accountMigrationDialog

    property string imgBase64: ""

    visible: false

    anchors.centerIn: parent.Center
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    contentItem: Rectangle{
        implicitWidth: 455
        implicitHeight: 594

        StackLayout{
            id: stackedWidget
            anchors.fill: parent

            currentIndex: 0

            Rectangle{
                id: accountMigrationPage

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.leftMargin: 11
                Layout.rightMargin: 11
                Layout.topMargin: 11
                Layout.bottomMargin: 11

                ColumnLayout{
                    spacing: 7

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    RowLayout{
                        Layout.fillWidth: true

                        Item{
                            Layout.fillWidth: true

                            Layout.minimumHeight: 20
                            Layout.preferredHeight: 20
                            Layout.maximumHeight: 20
                        }

                        Label{
                            id: accountMigrationLabel

                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: 27

                            font.pointSize: 13
                            font.kerning: true
                            wrapMode:Text.Wrap

                            text: qsTr("Account Migration Required")

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item{
                            Layout.fillWidth: true

                            Layout.minimumHeight: 20
                            Layout.preferredHeight: 20
                            Layout.maximumHeight: 20
                        }
                    }

                    RowLayout{
                        spacing: 7

                        Item{
                            Layout.fillWidth: true

                            Layout.minimumHeight: 20
                            Layout.preferredHeight: 20
                            Layout.maximumHeight: 20
                        }

                        Label{
                            id: migrationReasonLabel

                            Layout.alignment: Qt.AlignHCenter
                            Layout.preferredHeight: 21

                            font.pointSize: 10
                            font.kerning: true
                            wrapMode:Text.Wrap

                            text: qsTr("This account is malformed. Please enter your password")

                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }

                        Item{
                            Layout.fillWidth: true

                            Layout.minimumHeight: 20
                            Layout.preferredHeight: 20
                            Layout.maximumHeight: 20
                        }
                    }

                    Item{
                        Layout.minimumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.maximumWidth: 20

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
                    }

                    ColumnLayout{
                        spacing: 7

                        Layout.fillWidth: true

                        RowLayout{
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignCenter

                            Item{
                                Layout.fillWidth: true

                                Layout.minimumHeight: 20
                                Layout.preferredHeight: 20
                                Layout.maximumHeight: 20
                            }

                            Label{
                                id: avatarLabel

                                Layout.maximumWidth: 200
                                Layout.preferredWidth: 200
                                Layout.minimumWidth: 200

                                Layout.maximumHeight: 200
                                Layout.preferredHeight: 200
                                Layout.minimumHeight: 200

                                Layout.alignment: Qt.AlignHCenter

                                background: Rectangle {
                                    id: avatarLabelBackground

                                    anchors.fill: parent
                                    color: "transparent"

                                    Image{
                                        id: avatarImg

                                        anchors.fill: parent
                                        source: {
                                            if(imgBase64.length === 0){
                                                return ""
                                            } else {
                                                return "data:image/png;base64," + imgBase64
                                            }
                                        }
                                        fillMode: Image.PreserveAspectCrop
                                        layer.enabled: true
                                        layer.effect: OpacityMask {
                                            maskSource: Rectangle{
                                                width: avatarImg.width
                                                height: avatarImg.height
                                                radius: {
                                                    var size = ((avatarImg.width <= avatarImg.height)? avatarImg.width:avatarImg.height)
                                                    return size /2
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            Item{
                                Layout.fillWidth: true

                                Layout.minimumHeight: 20
                                Layout.preferredHeight: 20
                                Layout.maximumHeight: 20
                            }
                        }
                    }

                    Item{
                        Layout.minimumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.maximumWidth: 20

                        Layout.minimumHeight: 30
                        Layout.preferredHeight: 30
                        Layout.maximumHeight: 30
                    }

                    GridLayout{

                    }

                    Item{
                        Layout.minimumWidth: 20
                        Layout.preferredWidth: 20
                        Layout.maximumWidth: 20

                        Layout.minimumHeight: 40
                        Layout.preferredHeight: 40
                        Layout.maximumHeight: 40
                    }

                    RowLayout{
                        spacing: 0
                        Layout.fillWidth: true

                        HoverableGradientButton {
                            id: migrationPushButton

                            Layout.alignment: Qt.AlignLeft
                            Layout.maximumWidth: 100
                            Layout.preferredWidth: 100

                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 30

                            text: qsTr("Migrate")
                            font.pointSize: 10
                            font.kerning: true

                            radius: height / 2
                            backgroundColor: JamiTheme.releaseColor

                            onClicked: {

                            }
                        }

                        Item {
                            Layout.fillWidth: true

                            Layout.minimumHeight: 40
                            Layout.preferredHeight: 40
                            Layout.maximumHeight: 40
                        }

                        HoverableButtonTextItem {
                            id: deleteAccountPushButton

                            backgroundColor: "red"
                            onEnterColor: Qt.rgba(150 / 256, 0, 0, 0.7)
                            onDisabledBackgroundColor: Qt.rgba(
                                                           255 / 256,
                                                           0, 0, 0.8)
                            onPressColor: backgroundColor
                            textColor: "white"

                            Layout.alignment: Qt.AlignRight
                            Layout.maximumWidth: 100
                            Layout.preferredWidth: 100

                            Layout.maximumHeight: 30
                            Layout.preferredHeight: 30
                            Layout.minimumHeight: 30

                            text: qsTr("Delete")
                            font.pointSize: 10
                            font.kerning: true

                            radius: height / 2

                            onClicked: {

                            }
                        }
                    }
                }
            }

            Rectangle{
                id: migrationWaitingPage

                Layout.fillWidth: true
                Layout.fillHeight: true

                Layout.leftMargin: 11
                Layout.rightMargin: 11
                Layout.topMargin: 11
                Layout.bottomMargin: 11

                ColumnLayout{
                    anchors.fill: parent
                    spacing: 7

                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Layout.preferredHeight: 211
                    }

                    RowLayout{
                        spacing: 7

                        Layout.fillWidth: true

                        Item{
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Layout.minimumWidth: 20
                        }

                        Label{
                            id: spinnerLabel

                            Layout.alignment: Qt.AlignHCenter

                            Layout.maximumWidth: 200
                            Layout.preferredWidth: 200
                            Layout.minimumWidth: 200

                            Layout.maximumHeight: 200
                            Layout.preferredHeight: 200
                            Layout.minimumHeight: 200

                            background: Rectangle {
                                anchors.fill: parent
                                AnimatedImage {
                                    id: spinnerMovie

                                    anchors.fill: parent

                                    source: "qrc:/images/jami_eclipse_spinner.gif"

                                    playing: spinnerLabel.visible
                                    paused: false
                                    fillMode: Image.PreserveAspectFit
                                    mipmap: true
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                            Layout.fillHeight: true

                            Layout.minimumWidth: 20
                        }
                    }

                    Item{
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Layout.preferredHeight: 211
                    }

                    Label{
                        id: progressLabel

                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true

                        text: qsTr("Migrating your Jami account...")
                        font.pointSize: 11
                        font.kerning: true
                    }

                    Item{
                        Layout.fillWidth: true

                        Layout.minimumHeight: 20
                        Layout.preferredHeight: 20
                        Layout.maximumHeight: 20
                    }
                }
            }
        }
    }
}
