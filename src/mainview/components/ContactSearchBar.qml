import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import net.jami.constant.jamitheme 1.0

Rectangle {
    id: contactSearchBarRect

    signal contactSearchBarTextChanged(string text)

    function setPlaceholderString(str) {
        placeholderTextForSearchBar.text = str
    }

    function clearFocus() {
        fakeFocus.forceActiveFocus()
    }

    function clearText() {
        contactSearchBar.clear()
        fakeFocus.forceActiveFocus()
    }

    border.color: JamiTheme.pressColor
    radius: 10
    color: contactSearchBar.activeFocus ? "white" : JamiTheme.contactSearchBarPlaceHolderGreyBackground

    FocusScope {
        id: fakeFocus
    }

    Image {
        id: searchIconImage

        anchors.verticalCenter: contactSearchBarRect.verticalCenter
        anchors.left: contactSearchBarRect.left
        anchors.leftMargin: 5

        width: 20
        height: 20

        fillMode: Image.PreserveAspectFit
        mipmap: true
        source: "qrc:/images/icons/ic_baseline-search-24px.svg"
    }

    TextField {
        id: contactSearchBar

        anchors.verticalCenter: contactSearchBarRect.verticalCenter
        anchors.left: searchIconImage.right

        width: contactSearchBarRect.width - searchIconImage.width - 10
        height: contactSearchBarRect.height - 5

        font.pointSize: JamiTheme.textFontSize - 1
        selectByMouse: true
        selectionColor: JamiTheme.contactSearchBarPlaceHolderTextFontColor

        Text {
            id: placeholderTextForSearchBar

            anchors.verticalCenter: contactSearchBar.verticalCenter
            anchors.left: contactSearchBar.left
            anchors.leftMargin: 5

            text: qsTr("Find a new or existing contact")
            font.pointSize: JamiTheme.textFontSize - 1
            color: JamiTheme.contactSearchBarPlaceHolderTextFontColor
            visible: !contactSearchBar.text && !contactSearchBar.activeFocus
        }

        background: Rectangle {
            id: searchBarBackground

            color: "transparent"
        }

        onTextChanged: {
            contactSearchBarRect.contactSearchBarTextChanged(
                        contactSearchBar.text)
        }
    }
}
