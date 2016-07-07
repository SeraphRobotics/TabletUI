import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import "." 1.0
import "../../Components"

Item {
    visible: false
    anchors.fill: parent

    anchors {
        top : parent.header.bottom
        topMargin: 35
    }

    // clean user input in text fields
    function cleanUserInput()
    {
        anotherScanId.text = ""
        patientNameInput.text = ""
        commentsTextArea.text = ""
    }

    GrayDescriptionText {
        id : pageTitle

        anchors {
            left : parent.left
            leftMargin: 20
            top : parent.top
            topMargin: 30
        }
        font.pixelSize: 20

        text : qsTr("Files on USB Drive")
    }

    Item {
        id : headerContainer

        anchors {
            top : pageTitle.bottom
            topMargin: 30
            left : usbItemsList.left
            leftMargin: 0
        }

        width : usbItemsList.width-160
        height : textTimestamp.height+10


        GrayDescriptionText {
            id : textTimestamp

            font {
                pixelSize: 16
                bold: true
            }
            anchors {
                top : parent.top
                left : parent.left
                leftMargin: 60/560*parent.width
            }
            text : qsTr("Timestamp")
        }
        GrayDescriptionText {
            font {
                pixelSize: 16
                bold: true
            }
            anchors {
                top : parent.top
                left : parent.left
                leftMargin: 265/560*parent.width
            }
            text : qsTr("Type")
        }
        GrayDescriptionText {
            font {
                pixelSize: 16
                bold: true
            }
            text : qsTr("Status")

            anchors {
                top : parent.top
                left : parent.left
                leftMargin: 410/560*parent.width
            }
        }
    }

    ListView {
        id : usbItemsList

        highlightMoveVelocity : 0
        anchors {
            top : headerContainer.bottom
            topMargin: 2
            bottom : border.top
            bottomMargin: 100/700*parent.height
            horizontalCenter: parent.horizontalCenter
        }
        width : parent.width-40
        clip : true
        delegate: ChoosePatientScreenDelegate {
            listView : usbItemsList
        }

        Component.onCompleted: {
            model = qmlCppWrapper.usbModel
        }
    }

    Row {
        id : buttonsRow

        spacing : 100
        anchors {
            top : usbItemsList.bottom
            topMargin: 15
            horizontalCenter: parent.horizontalCenter
        }

        StyledButton {
            id : deleteButton

            anchors.verticalCenter: parent.verticalCenter
            width : 160
            titleText : qsTr("delete")

            onCustomClicked: {
                questionDialog.visible = true
            }
        }

        StyledButton {
            id : addScan

            width : 180
            height : 60
            titleText : qsTr("add scan to\nexisting patient")

            onCustomClicked: {
                if (!usbItemsList.model.isScan(usbItemsList.currentIndex)) {
                    messageDialog.text = qsTr("You cannot use Rx item as Scan. Please select Scan item.")
                    messageDialog.visible = true
                    messageDialog.calculateImplicitWidth()
                    return
                }

                var scanIds = [usbItemsList.model.id(usbItemsList.currentIndex),
                               anotherScanId.text]
                var patient = patientsListModel.getSpecificItem(patientsListModel.currentIndex)
                qmlCppWrapper.addScanToExistingPatient(scanIds, patient.id, patient.doctorId)
                cleanUserInput()
            }
        }
    }

    Rectangle {
        id : border

        height : 2
        anchors {
            left : parent.left
            leftMargin: 25
            right : parent.right
            rightMargin: 25
            verticalCenter: parent.verticalCenter
        }
        color : "lightgray"
    }

    Column {
        id: column
        spacing: 10

        anchors {
            top : border.bottom
            topMargin: 15
            left : buttonsRow.left
        }

        StyledButton {
            id : loadScan

            width : 200
            height : 60
            titleText : qsTr("load scan as \nnew patient")

            onCustomClicked: {
                if (patientNameInput.text.length == 0) {
                    patientNameInput.backgroundNormalBorderColor = "red"
                    return
                }
                patientNameInput.backgroundNormalBorderColor = "#cccccc"

                var patientName = patientNameInput.text
                if (patientName.length < qmlCppWrapper.patientNameMinLength) {
                    messageDialog.text = qsTr("You have specified too small patient name, please use at least %1 characters")
                    .arg(qmlCppWrapper.patientNameMinLength)
                    messageDialog.visible = true
                    messageDialog.calculateImplicitWidth()
                    return
                }

                if (patientsListModel.contains(patientName)) {
                    // user already has patient with the same name
                    messageDialog.text = qsTr("You already has patient with the same name")
                    messageDialog.visible = true
                    messageDialog.calculateImplicitWidth()
                    return
                }

                var comments = commentsTextArea.text
                var user = usersListModel.getSpecificItem(usersListModel.currentIndex)
                var scanIds = [usbItemsList.model.id(usbItemsList.currentIndex),
                               anotherScanId.text]
                qmlCppWrapper.newPatientFromScan(scanIds, patientName, comments, user.id)
                cleanUserInput()
            }
        }

        GrayDescriptionText {
            id : anotherScanDescription

            width : 200
            text : qsTr("another foot scan id")
            font.pixelSize: 15
            horizontalAlignment: Text.AlignHCenter
        }

        StyledTextInput {
            id : anotherScanId

            width : 200
            height : 40
            textColor: "#666666"
            welcomeTextColor : "#666666"
        }
    }

    GrayDescriptionText {
        id : patientNameInfo

        text : qsTr("patient name")
        font.pixelSize: 20

        anchors {
            top : border.bottom
            topMargin: 5
            left : column.right
            leftMargin: 70
        }
    }

    StyledTextInput {
        id : patientNameInput

        textColor: "#666666"
        welcomeTextColor : "#666666"

        anchors {
            top : patientNameInfo.bottom
            topMargin: 10
            left : patientNameInfo.left
            right : parent.right
            rightMargin: 80
        }
    }

    GrayDescriptionText {
        id : commentsTextInfo

        text : qsTr("comments")
        font.pixelSize: 20

        anchors {
            top : patientNameInput.bottom
            topMargin: 5
            left : column.right
            leftMargin: 70
        }
    }

    CommentsTextArea {
        id: commentsTextArea

        anchors {
            top : commentsTextInfo.bottom
            topMargin: 10
            left : patientNameInput.left
            right : patientNameInput.right
            bottom : parent.bottom
            bottomMargin: 20
        }
        text : usbItemsList.model.comment(usbItemsList.currentIndex)
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("SeraphRoboticsUi")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    MessageDialog {
        id: questionDialog
        title: qsTr("SeraphRoboticsUi")
        text: qsTr("Are you sure you want to delete this item?
                   \nThe corresponding data on usb disk will also be deleted.")
        icon: StandardIcon.Question
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: qmlCppWrapper.removeUsbItem(usbItemsList.currentIndex)
    }

    Connections {
        target: qmlCppWrapper

        onUsbModelChanged: {
            usbItemsList.model = ""
            usbItemsList.model = qmlCppWrapper.usbModel
        }
    }
}
