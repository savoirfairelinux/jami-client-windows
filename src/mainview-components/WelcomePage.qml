import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

import "../qmlcomponentutils"

Rectangle {
    id: welcomeRect

    Rectangle{
        id: welcomeRectComponentsGroup

        anchors.centerIn: parent

        width: Math.max(welcomePageGroupPreferedWidth, welcomeRect.width - 100)
        height: mainViewWindow.minimumHeight
        //width: parent.width - 100
        //height: parent.height - 200

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
                        font:   textMetricsjamiRegisteredNameText.font
                        text:   accountListModel.data(accountListModel.index(0,0), 258)
                        elideWidth: welcomeRectComponentsGroup.width - 100 - copyRegisterednameButton.width - qrCodeGenerateButton.width
                        elide: Qt.ElideMiddle
                    }

                    RoundButton {
                        id: copyRegisterednameButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30

                        Image {
                            id: copySVGImage

                            anchors.centerIn: copyRegisterednameButton

                            height: copyRegisterednameButton.height - 10
                            width: copyRegisterednameButton.width - 10

                            fillMode: Image.PreserveAspectFit
                            mipmap: true
                            source: "qrc:/images/icons/ic_content_copy.svg"
                        }

                        onClicked: {
                            qmlUtils.setText(textMetricsjamiRegisteredNameText.text)
                        }
                    }

                    RoundButton {
                        id: qrCodeGenerateButton

                        Layout.alignment: Qt.AlignCenter
                        Layout.preferredWidth: 30
                        Layout.preferredHeight: 30

                        Image {
                            id: qrPNGImage

                            anchors.centerIn: qrCodeGenerateButton

                            height: copyRegisterednameButton.height - 10
                            width: copyRegisterednameButton.width - 10

                            fillMode: Image.PreserveAspectFit
                            mipmap: true
                            source: "qrc:/images/qrcode.png"
                        }

                        onClicked: {
                            if(userQrImage.visible)
                                userQrImage.visible = false
                            else
                                userQrImage.visible = true
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

        RoundButton {
            id: aboutButton

            anchors.horizontalCenter: welcomeRectComponentsGroup.horizontalCenter
            anchors.bottom: welcomeRectComponentsGroup.bottom
            anchors.bottomMargin: 5

            width: 80
            height: 30

            radius: 10
            text: qsTr("About")
            font.pointSize: textFontSize

            onClicked: {
                aboutPopUpDialog.open()
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
