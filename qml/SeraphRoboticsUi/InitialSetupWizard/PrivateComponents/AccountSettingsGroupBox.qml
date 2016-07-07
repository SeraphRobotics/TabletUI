import QtQuick 2.4

import "../../Components"
import ".."

GroupBoxTemplate {
    id : accountSettings

    height : 220
    width : 510

    title: qsTr("Account Settings")

    property alias pinRequireValue : pinRequire.checked
    property alias loginByPrescriberValue : loginByPrescriber.checked
    property alias autoAssignPatientsValue : autoAssignPatients.checked

    Column {
        anchors {
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: 15
            horizontalCenter: parent.horizontalCenter
        }

        spacing : 10

        ToggleButton {
            id : pinRequire

            title: qsTr("require pin to login")
            checked : true
        }
        ToggleButton{
            id : loginByPrescriber

            title: qsTr("login by prescriber")
        }
        ToggleButton {
            id : autoAssignPatients

            title: qsTr("auto-assign patients")
            checked: true
        }
    }
}

