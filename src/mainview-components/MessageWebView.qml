import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.constant.color 1.0

Rectangle {
    id: messageWebViewRect

    signal backToWelcomeViewButtonClicked()

    Rectangle {
        id: messagingHeaderRect

        anchors.top: messageWebViewRect.top
        anchors.left: messageWebViewRect.left

        width: messageWebViewRect.width
        height: 60

        RowLayout {
            id: messagingHeaderRectRowLayout

            Button {
                id: backToWelcomeViewButton

                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                background: Rectangle {
                    id: backToWelcomeViewButtonBackground

                    color: ColorConstant.releaseColor

                    Image {
                        id: backImage

                        anchors.centerIn: backToWelcomeViewButtonBackground

                        height: backToWelcomeViewButtonBackground.height - 10
                        width: backToWelcomeViewButtonBackground.width - 10

                        fillMode: Image.PreserveAspectFit
                        mipmap: true
                        source: "qrc:/images/icons/ic_arrow_back_24px.svg"
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onPressed: { backToWelcomeViewButtonBackground.color = ColorConstant.pressColor; }
                        onReleased: { backToWelcomeViewButtonBackground.color = ColorConstant.releaseColor; messageWebViewRect.backToWelcomeViewButtonClicked()}
                        onEntered: { backToWelcomeViewButtonBackground.color = ColorConstant.hoverColor; }
                        onExited: { backToWelcomeViewButtonBackground.color = ColorConstant.releaseColor; }
                    }
                }
            }

        }
    }

    color: "pink"
}
