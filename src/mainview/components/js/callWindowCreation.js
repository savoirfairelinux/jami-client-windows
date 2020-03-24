//  map<accountId, map<convUid, callwindowpointer>>
let accountCallWindowMap = new Map()

var component;
var callWindow;

function createCallWindowObjects(accountId, convUid) {
    component = Qt.createComponent("../CallStackViewWindow.qml")
    if (component.status === Component.Ready)
        finishCreation(accountId, convUid)
    else
        component.statusChanged.connect(finishCreation)
}

function finishCreation(accountId, convUid) {
    if (component.status === Component.Ready) {

        callWindow = component.createObject()
        if (callWindow === null) {
            // Error Handling
            console.log("Error creating object for accountId" + accountId)
        }

        callWindow.responsibleConvUid = convUid
        callWindow.responsibleAccountId = accountId

        // set maps
        if(accountCallWindowMap.has(accountId)) {
            if(!accountCallWindowMap.get(accountId).has(convUid)) {
                accountCallWindowMap.get(accountId).set(convUid, callWindow)
            }
        } else {
            let callWindowMap = new Map()
            callWindowMap.set(convUid, callWindow)
            accountCallWindowMap.set(accountId, callWindowMap)
        }
    } else if (component.status === Component.Error) {
        // Error Handling
        console.log("Error loading component:", component.errorString())
    }
}

function showCallWindow(accountId, convUid) {
    if(accountCallWindowMap.has(accountId)) {
        if(accountCallWindowMap.get(accountId).has(convUid)) {
            accountCallWindowMap.get(accountId).get(convUid).show()
        }
    }
}

function closeCallWindow(accountId, convUid) {
    if(accountCallWindowMap.has(accountId)) {
        let callWindowMap = accountCallWindowMap.get(accountId)
        if(callWindowMap.has(convUid)) {
            callWindowMap.get(convUid).close()
            accountCallWindowMap.delete(callWindowMap)
        }
    }
}
