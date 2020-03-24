import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtQml 2.14
import net.jami.constant.jamitheme 1.0

import "../../commoncomponents"

Rectangle {
    id: callOverlayButtonGroupRect

    signal buttonEntered()

    signal chatButtonClicked()
    signal hangUpButtonClicked()
    signal holdButtonClicked()
    signal noMicButtonClicked()
    signal recButtonClicked()

    function holdButtonSetChecked(checked) {
        holdButton.setChecked(checked)
    }

    RowLayout {
        id: callOverlayButtonGroupRectRowLayout

        anchors.fill: parent

        TintedButton {
            id: hangUpButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.hangUpButtonTintedRed
            normalPixmapSource: "qrc:/images/icons/ic_close_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_close_white_24dp.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }

            onClicked: {
                callOverlayButtonGroupRect.hangUpButtonClicked()
            }
        }

        TintedButton {
            id: holdButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_pause_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_play_white_24dp.png"

            onClicked: {
                callOverlayButtonGroupRect.holdButtonClicked()
            }

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }

        TintedButton {
            id: addToConferenceButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_group_add_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_group_add_white_24dp.png"

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }

        TintedButton {
            id: chatButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_chat_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_chat_white_24dp.png"

            onClicked: {
                callOverlayButtonGroupRect.chatButtonClicked()
            }

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }

        TintedButton {
            id: noMicButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_mic_white_24dp.png"
            selectedPixmapSource: "qrc:/images/icons/ic_mic_off_white_24dp.png"

            onClicked: {
                callOverlayButtonGroupRect.noMicButtonClicked()
            }

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }

        TintedButton {
            id: recButton

            Layout.alignment: Qt.AlignCenter
            Layout.preferredHeight: 30
            Layout.preferredWidth : 30

            tintColor: JamiTheme.buttonTintedBlue
            normalPixmapSource: "qrc:/images/icons/ic_voicemail_white_24dp_2x.png"
            selectedPixmapSource: "qrc:/images/icons/ic_voicemail_white_24dp_2x.png"

            onClicked: {
                callOverlayButtonGroupRect.recButtonClicked()
            }

            onButtonEntered: {
                callOverlayButtonGroupRect.buttonEntered()
            }
        }
    }

    color: "transparent"
}
