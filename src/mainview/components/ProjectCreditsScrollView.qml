import QtQuick 2.14
import QtQuick.Controls 2.14
import net.jami.JamiTheme 1.0

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

        font.pointSize: JamiTheme.textFontSize - 3
        text: utilsAdapter.getProjectCredits()
        textFormat: TextEdit.RichText
    }

    background: Rectangle {
        id: projectCreditsScrollViewBackground

        radius: 5
        border.color: JamiTheme.tabbarBorderColor
    }
}
