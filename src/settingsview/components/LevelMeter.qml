import QtQuick 2.14
import QtQuick.Controls 2.15

ProgressBar {
    id: levelMeter

    value: {
        return clamp(rmsLevel * 300.0, 0.0, 100.0)
    }

    property real rmsLevel: 0

    function clamp(num,a,b){
        return Math.max(Math.min(num, Math.max(a, b)), Math.min(a, b))
    }

    function start(){
        rmsLevel = 0
    }

    function stop(){

    }

    function setLevel(rmsLevelIn){
        rmsLevel = rmsLevelIn
    }

}
