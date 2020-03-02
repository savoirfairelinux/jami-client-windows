import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

import "../qmlcomponentutils"

Rectangle {
    id: welcomeRect

    property int buttonPreferredSize: 30

    Rectangle{
        id: welcomeRectComponentsGroup

        anchors.centerIn: parent

        width: Math.max(welcomePageGroupPreferedWidth, welcomeRect.width - 100)
        height: mainViewWindow.minimumHeight

        ColumnLayout {
            id: welcomeRectComponentsGroupColumnLayout

            Image {
                id: jamiLogoImage

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: welcomeRectComponentsGroup.width
                Layout.preferredHeight: 100
                Layout.topMargin: 50
                Layout.bottomMargin: 10

                fillMode: Image.PreserveAspectFit
                source: "qrc:/images/logo-jami-standard-coul.png"
                mipmap: true
            }

            Label {
                id: jamiIntroText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: welcomeRectComponentsGroup.width
                Layout.preferredHeight: 100
                Layout.bottomMargin: 10

                wrapMode: Text.WordWrap
                font.pointSize: textFontSize + 1

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: qsTr("Jami is a free software for universal communication which repects the freedoms and privacy of its user.")

                background: Rectangle {
                    color: "yellow"
                }
            }

            Label {
                id: jamiShareWithFriendText

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: welcomeRectComponentsGroup.width
                Layout.preferredHeight: 50
                Layout.topMargin: 5

                wrapMode: Text.WordWrap
                font.pointSize: textFontSize - 1

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: qsTr("This is your ID.\nCopy and share it with your friends")

                background: Rectangle {
                    color: "orange"
                }
            }

            Rectangle{
                id: jamiRegisteredNameRect

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: welcomeRectComponentsGroup.width
                Layout.preferredHeight: 30
                Layout.bottomMargin: 10

                RowLayout{
                    id: jamiRegisteredNameRowLayout

                    TextEdit {
                        id: jamiRegisteredNameText

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: welcomeRectComponentsGroup.width - 100
                        Layout.preferredHeight: 30

                        selectByMouse: true
                        readOnly: true
                        font.pointSize: textFontSize

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter

                        text: textMetricsjamiRegisteredNameText.elidedText
                    }

                    TextMetrics {
                        id:     textMetricsjamiRegisteredNameText
                        font:   jamiRegisteredNameText.font
                        text:   accountListModel.data(accountListModel.index(0,0), 258)
                        elideWidth: welcomeRectComponentsGroup.width - 100 - copyRegisterednameButton.width - qrCodeGenerateButton.width
                        elide: Qt.ElideMiddle
                    }

                    Button {
                        id: copyRegisterednameButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: buttonPreferredSize
                        Layout.preferredHeight: buttonPreferredSize

                        background: Rectangle {
                            id: copyRegisterednameButtonBackground

                            radius: 30
                            color: ColorConstant.releaseColor

                            Image {
                                id: copySVGImage

                                anchors.centerIn: copyRegisterednameButtonBackground

                                height: copyRegisterednameButtonBackground.height - 10
                                width: copyRegisterednameButtonBackground.width - 10

                                fillMode: Image.PreserveAspectFit
                                mipmap: true
                                source: "qrc:/images/icons/ic_content_copy.svg"
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: { copyRegisterednameButtonBackground.color = ColorConstant.pressColor; }
                                onReleased: {
                                    copyRegisterednameButtonBackground.color = ColorConstant.releaseColor
                                    qmlUtils.setText(textMetricsjamiRegisteredNameText.text)
                                }
                                onEntered: { copyRegisterednameButtonBackground.color = ColorConstant.hoverColor; }
                                onExited: { copyRegisterednameButtonBackground.color = ColorConstant.releaseColor; }
                            }
                        }
                    }

                    Button {
                        id: qrCodeGenerateButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: buttonPreferredSize
                        Layout.preferredHeight: buttonPreferredSize

                        background: Rectangle {
                            id: qrCodeGenerateButtonBackground

                            radius: 30
                            color: ColorConstant.releaseColor

                            Image {
                                id: qrPNGImage

                                anchors.centerIn: qrCodeGenerateButtonBackground

                                height: qrCodeGenerateButtonBackground.height - 10
                                width: qrCodeGenerateButtonBackground.width - 10

                                fillMode: Image.PreserveAspectFit
                                mipmap: true
                                source: "qrc:/images/qrcode.png"
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: { qrCodeGenerateButtonBackground.color = ColorConstant.pressColor; }
                                onReleased: {
                                    qrCodeGenerateButtonBackground.color = ColorConstant.releaseColor
                                    if(userQrImage.visible)
                                        userQrImage.visible = false
                                    else
                                        userQrImage.visible = true
                                }
                                onEntered: { qrCodeGenerateButtonBackground.color = ColorConstant.hoverColor; }
                                onExited: { qrCodeGenerateButtonBackground.color = ColorConstant.releaseColor; }
                            }
                        }
                    }
                }
            }

            Image {
                id: userQrImage

                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: 100
                Layout.preferredHeight: 100

                visible: false
                fillMode: Image.PreserveAspectFit
                source: "image://accountImage/account_0_0_qr"
                // requestedSize
                sourceSize.width: 100
                sourceSize.height: 100
                mipmap: true
            }
        }

        Button {
            id: aboutButton

            anchors.horizontalCenter: welcomeRectComponentsGroup.horizontalCenter
            anchors.bottom: welcomeRectComponentsGroup.bottom
            anchors.bottomMargin: 5

            width: 80
            height: buttonPreferredSize

            text: qsTr("About")
            font.pointSize: textFontSize

            background: Rectangle {
                id: aboutButtonBackground

                radius: 10
                color: ColorConstant.releaseColor

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: { aboutButtonBackground.color = ColorConstant.pressColor; }
                    onReleased: { aboutButtonBackground.color = ColorConstant.releaseColor; aboutPopUpDialog.open(); }
                    onEntered: { aboutButtonBackground.color = ColorConstant.hoverColor; }
                    onExited: { aboutButtonBackground.color = ColorConstant.releaseColor; }
                }
            }
        }
        color: "blue"
    }

    CustomBorder {
        commonBorder: false
        lBorderwidth: 1
        rBorderwidth: 0
        tBorderwidth: 0
        bBorderwidth: 0
        borderColor: ColorConstant.tabbarBorderColor
    }
    color: "pink"
}
