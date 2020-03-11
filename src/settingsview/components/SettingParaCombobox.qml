import QtQuick 2.15
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.14
import QtQuick.Controls.Styles 1.4

ComboBox {
    id: paraBox

    function clear(){
        listmodelOfBox.clear()
    }

    function append(dict){
        listmodelOfBox.append()
    }

    function get(index){
        return listmodelOfBox.get(index)
    }

    function insert(index, dict){
        listmodelOfBox.insert(index, dict)
    }

    function move(from, to, n){
        listmodelOfBox.move(from, to, n)
    }
    function remove(index, count = 1){
        listmodelOfBox.remove(index, count)
    }
    function set(index, dict){
        listmodelOfBox.set(index, dict)
    }
    function setProperty(index, propertyIn, valueIn){
        listmodelOfBox.setProperty(index, propertyIn, valueIn)
    }

    model: ListModel{
        id: listmodelOfBox
    }
}
