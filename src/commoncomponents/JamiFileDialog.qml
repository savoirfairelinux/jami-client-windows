import QtQuick 2.14
import QtQuick.Dialogs 1.3

FileDialog {
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.home
}
