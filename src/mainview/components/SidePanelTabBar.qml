
/*
 * Copyright (C) 2020 by Savoir-faire Linux
 * Author: Mingrui Zhang <mingrui.zhang@savoirfairelinux.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.Models 1.0
import "../../commoncomponents"

TabBar {
    id: tabBar

    property alias converstationTabDown: pageOne.down
    property alias invitationTabDown: pageTwo.down
    property alias converstationTabWidth: pageOne.width
    property alias invitationTabWidth: pageTwo.width
    property alias converstationTabHeight: pageOne.height
    property alias invitationTabHeight: pageTwo.height

    visible: tabBarVisible

    currentIndex: 0

    TabButton {

        id: pageOne
        down: true

        Rectangle {
            id: totalUnreadMessagesCountRect

            anchors.centerIn: buttonRectOne
            anchors.verticalCenterOffset : -20
            anchors.horizontalCenterOffset : 15

            width: 16
            height: 16

            visible: totalUnreadMessagesCount > 0

            Text {
                id: totalUnreadMessagesCountText

                anchors.centerIn: totalUnreadMessagesCountRect

                text: totalUnreadMessagesCount > 9 ? "···" : totalUnreadMessagesCount
                color: "white"
                font.pointSize: JamiTheme.textFontSize
            }
            radius: 30
            color: JamiTheme.notificationRed

        }

        background: Rectangle {

            id: buttonRectOne
            width: tabBar.width / 2 + 1
            height: tabBar.height
            color: JamiTheme.backgroundColor

            Image {
                anchors.horizontalCenter: buttonRectOne.horizontalCenter
                anchors.top: buttonRectOne.top
                anchors.topMargin: 12

                width: 24
                height: 24

                fillMode: Image.PreserveAspectFit
                mipmap: true
                source: "qrc:/images/icons/baseline-people-24px.svg"
                opacity: enabled ? 0.8 : 0.3
            }

            Text {
                id: textConvElement

                anchors.horizontalCenter: buttonRectOne.horizontalCenter
                anchors.bottom: buttonRectOne.bottom
                anchors.bottomMargin: 16

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: qsTr("CONVERSATIONS")
                font.pointSize: JamiTheme.textFontSize
                opacity: enabled ? 1.0 : 0.3
            }

            Rectangle {
                id: markerTabOne
                width: buttonRectOne.width
                anchors.bottom: buttonRectOne.bottom
                height: 2
                color: pageOne.down == true ? JamiTheme.blue_ : "transparent"
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPressed: {
                    ConversationsAdapter.setConversationFilter("")
                    contactSearchBar.setPlaceholderString(
                                JamiTheme.contactSearchBarPlaceHolderConversationText)
                    pageOne.down = true
                    pageTwo.down = false
                    setCurrentUidSmartListModelIndex()
                    forceReselectConversationSmartListCurrentIndex()
                }
                onReleased: {
                    buttonRectOne.color = JamiTheme.backgroundColor
                }
                onEntered: {
                    buttonRectOne.color = JamiTheme.hoverColor
                }
                onExited: {
                    buttonRectOne.color = JamiTheme.backgroundColor
                }
            }
        }
    }

    TabButton {

        id: pageTwo

        Rectangle {
            id: pendingRequestCountRect

            anchors.centerIn: buttonRectTwo
            anchors.verticalCenterOffset : -20
            anchors.horizontalCenterOffset : 15

            width: 16
            height: 16

            visible: pendingRequestCount > 0

            Text {
                id: pendingRequestCountText

                anchors.centerIn: pendingRequestCountRect

                text: pendingRequestCount > 9 ? "···" : pendingRequestCount
                color: JamiTheme.backgroundColor
                font.pointSize: JamiTheme.textFontSize
            }
            radius: 30
            color: JamiTheme.notificationRed
        }

        background: Rectangle {
            id: buttonRectTwo

            width: tabBar.width / 2
            height: tabBar.height
            color: JamiTheme.backgroundColor

            Image {
                anchors.horizontalCenter: buttonRectTwo.horizontalCenter
                anchors.top: buttonRectTwo.top
                anchors.topMargin: 10

                width: 24
                height: 24

                fillMode: Image.PreserveAspectFit
                mipmap: true
                source: "qrc:/images/icons/drafts-24px.svg"
                opacity: enabled ? 0.8 : 0.3
            }

            Text {
                id: textInvElement

                anchors.horizontalCenter: buttonRectTwo.horizontalCenter
                anchors.bottom: buttonRectTwo.bottom
                anchors.bottomMargin: 16

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                font.pointSize: JamiTheme.textFontSize

                text: qsTr("INVITATIONS")
                opacity: enabled ? 1.0 : 0.3
            }

            Rectangle {
                id: markerTabTwo
                width: buttonRectTwo.width
                anchors.bottom: buttonRectTwo.bottom
                height: 2
                color: pageTwo.down == true ? JamiTheme.blue_ : "transparent"
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPressed: {
                    ConversationsAdapter.setConversationFilter("PENDING")
                    contactSearchBar.setPlaceholderString(
                                JamiTheme.contactSearchBarPlaceHolderInivitionText)
                    pageTwo.down = true
                    pageOne.down = false
                }
                onReleased: {
                    buttonRectTwo.color = JamiTheme.backgroundColor
                }
                onEntered: {
                    buttonRectTwo.color = JamiTheme.hoverColor
                }
                onExited: {
                    buttonRectTwo.color = JamiTheme.backgroundColor
                }
            }
        }
    }
}
