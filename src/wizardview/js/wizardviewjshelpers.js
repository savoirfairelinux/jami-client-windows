function oneShotConnect(sig, slot) {
    var f = function () {
        slot.apply(this, arguments)
        sig.disconnect(f)
    }
    sig.connect(f)
}
