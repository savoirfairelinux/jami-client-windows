import QtQuick 2.14
import QtQuick.Controls 2.15

ProgressBar {
    id: levelMeter

    property real redrawIntervalMs : 16
    property real rmsThreshold : 0.01

    property real rmsLevel: 0
    property real currentLevel: 0

    function clamp(num,a,b){
        Math.max(Math.min(num, Math.max(a, b)), Math.min(a, b))
    }

    function start(){
        rmsLevel = 0
        currentLevel = 0
        redrawTimerConnection.enable = true
        redrawTimer.restart()
    }

    function stop(){
        redrawTimerConnection.enable = false
        redrawTimer.stop()
    }

    function setLevel(rmsLevelIn){
        rmsLevel = rmsLevelIn
        update()
    }

    function redrawTimerExpired(){
        animateLevel()
        value = clamp(currentLevel * 100.0, 0.0, 100.0)
        update()
    }

    function animateLevel(){
        var distance = rmsLevel - currentLevel
            var velocity = 0.5 * (distance)
            if (Math.abs(distance) <= rmsThreshold) {
                currentLevel = rmsLevel
                return
            }
            currentLevel += velocity
    }

    Timer{
        id: redrawTimer

        interval: redrawIntervalMs
    }

    Connections{
        id: redrawTimerConnection

        target: redrawTimer

        function onTriggered(){
            redrawTimerExpired()
        }
    }
}
