import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

ComboBox {
    id: control

    function clear(){
        model.clear()
    }

    function append(dict){
        model.append(dict)
    }

    function get(index){
        return model.get(index)
    }
    function insert(index, dict){
        model.insert(index, dict)
    }
    function move(from, to, n){
        model.move(from, to, n)
    }
    function remove(index, count = 1){
        model.remove(index, count)
    }
    function set(index, dict){
        model.set(index, dict)
    }
    function setProperty(index, stringProperty, value){
        model.setProperty(index, stringProperty, value)
    }
    function sync(){
        model.sync()
    }

    textRole: "textDisplay"
    model: ListModel {
        id: modelOfControl

        //ListElement { textDisplay: "1920*1080 [60 fps]"; firstArg: "1920*1080"; secondArg: 60 }
    }

    delegate: ItemDelegate {
        width: control.width
        contentItem: Text {
            text: textDisplay
            color: "black"
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex === index
    }

    indicator: Canvas {
        id: canvas
        x: control.width - width - control.rightPadding
        y: control.topPadding + (control.availableHeight - height) / 2
        width: 12
        height: 8
        contextType: "2d"

        Connections {
            target: control
            function onPressedChanged(){
                canvas.requestPaint()
            }
        }

        onPaint: {
            context.reset();
            context.moveTo(0, 0);
            context.lineTo(width, 0);
            context.lineTo(width / 2, height);
            context.closePath();
            context.fillStyle = control.pressed ? "#17a81a" : "#21be2b";
            context.fill();
        }
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: control.indicator.width + control.spacing

        text: control.displayText
        font: control.font
        color: "black"
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        border.color: "white"
        border.width: control.visualFocus ? 2 : 1
        radius: 2
    }

    popup: Popup {
        y: control.height - 1
        width: control.width
        implicitHeight: contentItem.implicitHeight
        padding: 1

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model:  control.delegateModel
            currentIndex: control.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            border.color: "gray"
            radius: 2
        }
    }
}
