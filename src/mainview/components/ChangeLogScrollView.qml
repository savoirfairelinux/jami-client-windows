import QtQuick 2.14
import QtQuick.Controls 2.14
import net.jami.JamiTheme 1.0

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

        font.pointSize: JamiTheme.textFontSize - 3
        text: utilsAdapter.getChangeLog()
        textFormat: TextEdit.RichText
    }

    background: Rectangle {
        id: changeLogScrollViewBackground

        radius: 5
        border.color: JamiTheme.tabbarBorderColor
    }
}
