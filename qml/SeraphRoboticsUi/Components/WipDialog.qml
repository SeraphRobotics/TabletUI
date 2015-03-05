import QtQuick 2.4
import QtQuick.Dialogs 1.2

MessageDialog {
    id: wipDialog
    title: "W.I.P"
    text: "this item is work in progress"
    detailedText: ""
    onAccepted: {
        detailedText = ""
    }
    function show(id) {
        if(id.length !== 0)
            detailedText = "qml element: " + id
        open()
    }
}
