// map<accountId, map<convUid, callwindowpointer>>
let incomingCallPageWindowMap = new Map()

var incomingCallPageWindowComponent;
var incomingCallPageWindowObject;

function createincomingCallPageWindowObjects(accountId, convUid) {
    // check if the corrsponding call exists or not
    if(incomingCallPageWindowMap.has(accountId)) {
        if(incomingCallPageWindowMap.get(accountId).has(convUid)) {
            return
        }
    }

    incomingCallPageWindowComponent = Qt.createComponent("../components/IncomingCallPage.qml")
    if (incomingCallPageWindowComponent.status === Component.Ready)
        finishCreation(accountId, convUid)
    else
        incomingCallPageWindowComponent.statusChanged.connect(finishCreation)
}

function finishCreation(accountId, convUid) {
    if (incomingCallPageWindowComponent.status === Component.Ready) {
        incomingCallPageWindowObject = incomingCallPageWindowComponent.createObject()
        if (incomingCallPageWindowObject === null) {
            // Error Handling
            console.log("Error creating object for accountId" + accountId)
        }

        incomingCallPageWindowObject.responsibleConvUid = convUid
        incomingCallPageWindowObject.responsibleAccountId = accountId
        incomingCallPageWindowObject.updateUI()

        // set maps
        if(incomingCallPageWindowMap.has(accountId)) {
            incomingCallPageWindowMap.get(accountId).set(convUid, incomingCallPageWindowObject)
        } else {
            let incomingCallPageWindowTempMap = new Map()
            incomingCallPageWindowTempMap.set(convUid, incomingCallPageWindowObject)
            incomingCallPageWindowMap.set(accountId, incomingCallPageWindowTempMap)
        }
    } else if (incomingCallPageWindowComponent.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", incomingCallPageWindowComponent.errorString())
    }
}

function showIncomingCallPageWindow(accountId, convUid) {
    if(incomingCallPageWindowMap.has(accountId)) {
        if(incomingCallPageWindowMap.get(accountId).has(convUid)) {
            incomingCallPageWindowMap.get(accountId).get(convUid).show()
        }
    }
}

function closeIncomingCallPageWindow(accountId, convUid) {
    if(incomingCallPageWindowMap.has(accountId)) {
        let incomingCallPageWindowTempMap = incomingCallPageWindowMap.get(accountId)
        if(incomingCallPageWindowTempMap.has(convUid)) {
            var incomingCallPageWindow = incomingCallPageWindowTempMap.get(convUid)
            incomingCallPageWindow.close()
            incomingCallPageWindow.destroy()
            incomingCallPageWindowTempMap.delete(convUid)
            if(incomingCallPageWindowTempMap.size === 0) {
                incomingCallPageWindowMap.delete(accountId)
            } else {
                incomingCallPageWindowMap.set(accountId, incomingCallPageWindowTempMap)
            }
        }
    }
}
