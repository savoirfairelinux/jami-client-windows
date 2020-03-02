import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

import "../qmlcomponentutils"

Dialog {
    id: aboutPopUp

    // when dialog is opened, trigger mainViewWindow overlay which is defined in overlay.model (model : true is necessary)
    modal: true

    // content height + margin
    contentHeight: aboutPopUpContentRectColumnLayout.height + 5 * 7

    Component {
        id: changeLogScrollViewComponent

        ScrollView {
            id: changeLogScrollView

            anchors.fill: parent

            clip: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            TextEdit {
                id: changeLogTextArea

                width: changeLogScrollView.width

                readOnly: true
                wrapMode: Text.WordWrap

                font.pointSize: textFontSize - 3
                text: qmlUtils.getChangeLog()
                textFormat: TextEdit.RichText
            }

            background: Rectangle {
                id: changeLogScrollViewBackground

                radius: 5
                border.color: "yellow"
            }
        }
    }

    Component {
        id: projectCreditsScrollViewComponent

        ScrollView {
            id: projectCreditsScrollView

            anchors.fill: parent

            clip: true

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            TextEdit {
                id: projectCreditsTextArea

                horizontalAlignment: Text.AlignHCenter

                width: projectCreditsScrollView.width

                readOnly: true
                wrapMode: Text.WordWrap

                font.pointSize: textFontSize - 3
                text: qmlUtils.getProjectCredits()
                textFormat: TextEdit.RichText
            }

            background: Rectangle {
                id: projectCreditsScrollViewBackground

                radius: 5
                border.color: "yellow"
            }
        }
    }

    Rectangle{
        id: aboutPopUpContentRect

        anchors.fill: parent

        ColumnLayout {
            id:aboutPopUpContentRectColumnLayout

            Image {
                id: aboutPopUPJamiLogoImage

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: 100

                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/logo-jami-standard-coul.png"
                mipmap: true
            }

            Label {
                id: jamiVersionText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: textMetricsjamiVersionText.boundingRect.height

                font.pointSize: textFontSize - 2

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiVersionText.text

                TextMetrics {
                    id: textMetricsjamiVersionText
                    font: jamiVersionText.font
                    text: qsTr("version") + ": " + qmlUtils.getVersionStr()
                }

                background: Rectangle {
                    color: "yellow"
                }
            }

            Label {
                id: jamiSlogansText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: textMetricsjamiSlogansText.boundingRect.height
                Layout.topMargin: 5

                wrapMode: Text.WordWrap
                font.pointSize: textFontSize - 2

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiSlogansText.text

                TextMetrics {
                    id: textMetricsjamiSlogansText
                    font: jamiSlogansText.font
                    text: qsTr("Free as in Freedom")
                }

                background: Rectangle {
                    color: "orange"
                }
            }

            Label {
                id: jamiDeclarationText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: 40
                Layout.topMargin: 5

                wrapMode: Text.WordWrap
                font.pointSize: textFontSize - 2

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                // TextMetrics does not work for multi-line
                text: qsTr("The Microsoft Windows client for Jami.\nJami is a secured and distributed communciation software.")

                background: Rectangle {
                    color: "pink"
                }
            }

            Label {
                id: jamiDeclarationHyperText

                Layout.alignment: Qt.AlignCenter
                // strangely, hoveredLink works badly when width grows too large
                Layout.preferredWidth: 50
                Layout.preferredHeight: textMetricsjamiDeclarationHyperText.boundingRect.height
                Layout.topMargin: 5
                Layout.bottomMargin: 5

                font.pointSize: textFontSize - 2

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiDeclarationHyperText.text
                onLinkActivated: Qt.openUrlExternally(link)

                TextMetrics {
                    id: textMetricsjamiDeclarationHyperText
                    font: jamiDeclarationHyperText.font
                    text: '<html><style type="text/css"></style><a href="https://jami.net">jami.net</a></html>'
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }

                background: Rectangle {
                    color: "pink"
                }
            }

            Label {
                id: jamiDeclarationYearText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: textMetricsjamiDeclarationYearText.boundingRect.height
                Layout.bottomMargin: 5

                font.pointSize: textFontSize - 2

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiDeclarationYearText.text

                TextMetrics {
                    id: textMetricsjamiDeclarationYearText
                    font: jamiDeclarationYearText.font
                    text: "© 2015-2020 Savoir-faire Linux"
                }

                background: Rectangle {
                    color: "pink"
                }
            }

            Label {
                id: jamiNoneWarrantyHyperText

                Layout.alignment: Qt.AlignCenter
                // strangely, hoveredLink works badly when width grows too large
                Layout.preferredWidth: Math.min(300, aboutPopUpContentRect.width)
                // TextMetrics does not work for multi-lines
                Layout.preferredHeight: textMetricsjamiNoneWarrantyHyperText.boundingRect.height * 2
                Layout.bottomMargin: 10

                wrapMode: Text.WordWrap
                font.pointSize: textFontSize - 3

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: textMetricsjamiNoneWarrantyHyperText.text
                onLinkActivated: Qt.openUrlExternally(link)

                TextMetrics {
                    id: textMetricsjamiNoneWarrantyHyperText
                    font: jamiDeclarationHyperText.font
                    text: '<html><style type="text/css"></style>This program comes with absolutely no warranty.<br\>See the <a href="http://www.gnu.org/licenses/gpl-3.0.html">GNU General Public License, version 3 or later</a> for details.</html>'
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // we don't want to eat clicks on the Text
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }

                background: Rectangle {
                    color: "yellow"
                }
            }

            Rectangle {
                id: buttonGroupChangeLogAndCredits

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: 30
                Layout.bottomMargin: 10

                RowLayout {
                    id: buttonGroupChangeLogAndCreditsRowLayout

                    anchors.fill: parent

                    Button {
                        id: changeLogButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: textMetricschangeLogButton.boundingRect.width + 20
                        Layout.preferredHeight: 30

                        font.pointSize: textFontSize - 2
                        text: textMetricschangeLogButton.text

                        TextMetrics {
                            id: textMetricschangeLogButton
                            font: changeLogButton.font
                            text: qsTr("Change Log")
                        }

                        background: Rectangle {
                            id: changeLogButtonBackground

                            radius: 10
                            color: ColorConstant.releaseColor

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: { changeLogButtonBackground.color = ColorConstant.pressColor; }
                                onReleased: {
                                    changeLogButtonBackground.color = ColorConstant.releaseColor
                                    if(changeLogOrCreditsStack.depth > 1) {
                                        changeLogOrCreditsStack.pop()
                                    }
                                }
                                onEntered: { changeLogButtonBackground.color = ColorConstant.hoverColor; }
                                onExited: { changeLogButtonBackground.color = ColorConstant.releaseColor; }
                            }
                        }
                    }

                    Button {
                        id: creditsButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: changeLogButton.width
                        Layout.preferredHeight: 30

                        font.pointSize: textFontSize - 2
                        text: qsTr("Credits")

                        background: Rectangle {
                            id: creditsButtonBackground

                            radius: 10
                            color: ColorConstant.releaseColor

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: { creditsButtonBackground.color = ColorConstant.pressColor; }
                                onReleased: {
                                    creditsButtonBackground.color = ColorConstant.releaseColor
                                    if(changeLogOrCreditsStack.depth == 1) {
                                        changeLogOrCreditsStack.push(projectCreditsScrollViewComponent)
                                    }
                                }
                                onEntered: { creditsButtonBackground.color = ColorConstant.hoverColor; }
                                onExited: { creditsButtonBackground.color = ColorConstant.releaseColor; }
                            }
                        }
                    }
                }
                color: "purple"
            }

            StackView {
                id: changeLogOrCreditsStack

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: aboutPopUpContentRect.width
                Layout.preferredHeight: 150
                Layout.bottomMargin: 5

                initialItem: changeLogScrollViewComponent

                clip: true
            }
        }
    }

    background: Rectangle {
        border.width: 0
        radius: 10
    }
}
