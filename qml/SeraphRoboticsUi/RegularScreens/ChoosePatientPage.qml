import QtQuick 2.4

import ".."
import "../Components"
import "ChoosePatientScreenPrivateComponents"

// Page 14 from pdf.
ChoosePatientScreenMainWindow {


    ChoosePatientScreenPatientsList
    {
        id : patientsList
    }

    GroupBoxTemplate {
        id : manageUsbDrive

        property real widthScaleValue: (700/1280)

        width : widthScaleValue*parent.width

        title: qsTr("Manage USB Drive")
        state : qmlCppWrapper.usbDetected ? "usbDetected" : "usbNotDetected"

        anchors {
            top : parent.top
            topMargin: patientsList.anchors.topMargin
            right : parent.right
            rightMargin: 30
            bottom : parent.bottom
            bottomMargin : patientsList.anchors.bottomMargin
        }

        ChoosePatientScreenUsbDetected
        {
            id : manageUsbView
        }

        Image {
            id : noUsbDetectedImage

            width : sourceSize.width
            height : sourceSize.height

            visible : false

            anchors {
                centerIn: parent
            }

            smooth: true
            source : "qrc:/QmlResources/noUsbDetected.png"
        }

        states: [
            State {
                name: "usbDetected"
                PropertyChanges { target: manageUsbView; visible: true }
                PropertyChanges { target: noUsbDetectedImage; visible: false }
            },
            State {
                name: "usbNotDetected"
                PropertyChanges { target: manageUsbView; visible: false }
                PropertyChanges { target: noUsbDetectedImage; visible: true }
            }
        ]
    }

    rightNavigationButton.onButtonClicked:
    {
        stateManager.setState("patientHistoryScreen")
    }

    Connections {
        target: qmlCppWrapper

        onUsbConnected: {
            manageUsbDrive.state = "usbDetected"
        }

        onUsbDisconnected: {
            manageUsbDrive.state = "usbNotDetected"
        }
    }
}
