import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls.Universal 2.12
import QtGraphicalEffects 1.14

ApplicationWindow {
    id: mainApplicationWindow

    property int minWidth: 500
    property int minHeight: 500
    property int textPointSize: 8

    Universal.theme: Universal.Dark

    color: "green"

    visible: true
    width: 600
    minimumWidth: minWidth
    minimumHeight: minHeight
}
