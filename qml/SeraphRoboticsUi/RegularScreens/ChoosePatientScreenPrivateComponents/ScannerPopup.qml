pragma Singleton

import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import "../../Components"

Item {
    id : background

    anchors {
        fill: parent
        centerIn: parent
    }

    z : opacity === 1 ? 9999 : 0
    opacity : 0
    enabled: opacity === 1 ? true : false

    function setOpacity(opacityValue)
    {
        background.opacity = opacityValue
    }

    // clean user input in text fields
    function cleanUserInput()
    {
        leftScanId.text = ""
        rightScanId.text = ""
        patientNameInput.text = ""
        commentsTextArea.text = ""
    }

    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        id : popupWindow

        state: "hideScanSettings"

        width: 500
        height: 220

        radius : 10
        smooth : true

        border {
            width : 1
            color: "#b3b3b3"
        }

        anchors.centerIn: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#ffffff" }
            GradientStop { position:1.0; color: "#e8e9ea" }
        }

        Text {
            anchors {
                top : parent.top
                topMargin: 10
                right: parent.right
                rightMargin: 10
            }
            text: qsTr("X")
            color : "#666666"

            font {
                pixelSize: 20
            }

            MouseArea {
                anchors.fill : parent
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    cleanUserInput()
                    popupWindow.state = "hideScanSettings"
                    progressBar.visible = false
                    background.opacity = 0
                    qmlCppWrapper.cleanScanner()
                }
            }
        }

        GrayDescriptionText {
            id : descriptionText

            anchors {
                top : parent.top
                topMargin: 30
                horizontalCenter:  parent.horizontalCenter
            }

            color : "gray"
            font.pixelSize: 15

            text : qsTr("<p align=\"center\">Scanner is detected. Would you like to start scanning?</p>")
        }

        Row {
            id: scansRow
            spacing: 30

            anchors {
                horizontalCenter: parent.horizontalCenter
                top  : descriptionText.bottom
                topMargin : 30
            }

            Column {
                spacing : 15

                StyledButton {
                    id : startScanButton

                    width : 220
                    height : 50
                    titleText : qsTr("Start scanning")

                    onCustomClicked: {
                        qmlCppWrapper.startScanner()
                    }
                }

                StyledButton {
                    id : stopScanButton
                    visible: !startScanButton.visible

                    width : 220
                    height : 50
                    titleText : qsTr("Stop scanning")

                    onCustomClicked: {
                        startScanButton.visible = true
                        progressBar.value = 0
                        qmlCppWrapper.cleanScanner()
                    }
                }

                ProgressBar {
                    id: progressBar
                    visible: false
                    width : 220
                    height : 30

                    minimumValue: 0
                    maximumValue: 100
                }

                GrayDescriptionText {
                    id : resultText
                    visible: false

                    color : "gray"
                    font.pixelSize: 15
                }

                StyledButton {
                    id : addScanToPatient

                    visible: false

                    width : 220
                    height : 70

                    titleText: qsTr("add scan to\nexisting patient")

                    onCustomClicked: {
                        if (leftScanId.text.length == 0) {
                            leftScanId.backgroundNormalBorderColor = "red"
                            return
                        }
                        leftScanId.backgroundNormalBorderColor = "#cccccc"

                        var scanIds = [leftScanId.text, rightScanId.text]
                        var patient = patientsListModel.getSpecificItem(patientsListModel.currentIndex)
                        qmlCppWrapper.addScanToExistingPatient(scanIds, patient.id, patient.doctorId)
                        cleanUserInput()
                        background.opacity = 0
                    }
                }

                StyledButton {
                    id : saveScanToNewPatient

                    visible: false

                    width : 220
                    height : 70

                    titleText: qsTr("load scan as\nnew patient")

                    onCustomClicked: {
                        if (leftScanId.text.length == 0) {
                            leftScanId.backgroundNormalBorderColor = "red"
                            return
                        }
                        leftScanId.backgroundNormalBorderColor = "#cccccc"

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
                        var scanIds = [leftScanId.text, rightScanId.text]
                        qmlCppWrapper.newPatientFromScan(scanIds, patientName, comments, user.id)
                        cleanUserInput()
                        background.opacity = 0
                    }
                }

                GrayDescriptionText {
                    id : leftScanDescription

                    visible: false
                    width : 220
                    text : qsTr("foot scan id")
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                StyledTextInput {
                    id : leftScanId

                    visible: false
                    width : 220

                    textColor: "#666666"
                    welcomeTextColor : "#666666"
                }

                GrayDescriptionText {
                    id : rightScanDescription

                    visible: false
                    width : 220
                    text : qsTr("another foot scan id")
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                StyledTextInput {
                    id : rightScanId

                    visible: false
                    width : 220

                    textColor: "#666666"
                    welcomeTextColor : "#666666"
                }

            }

            Column {
                spacing : 15

                GrayDescriptionText {
                    id : patientNameInfo

                    visible: false
                    text : qsTr("patient name")
                    font.pixelSize: 20
                }

                StyledTextInput {
                    id : patientNameInput

                    visible: false

                    textColor: "#666666"
                    welcomeTextColor : "#666666"

                    anchors.right : parent.right
                    anchors.left : parent.left
                }

                GrayDescriptionText {
                    id : commentsTextInfo

                    visible: false
                    text : qsTr("comments")
                    font.pixelSize: 20
                }

                CommentsTextArea {
                    id: commentsTextArea

                    visible: false
                }
            }
        }

        states: [
            State {
                name: "hideScanSettings"
                PropertyChanges { target: resultText; visible: false }
                PropertyChanges { target: addScanToPatient; visible: false }
                PropertyChanges { target: saveScanToNewPatient; visible: false }
                PropertyChanges { target: leftScanDescription; visible: false }
                PropertyChanges { target: leftScanId; visible: false }
                PropertyChanges { target: rightScanDescription; visible: false }
                PropertyChanges { target: rightScanId; visible: false }
                PropertyChanges { target: patientNameInfo; visible: false }
                PropertyChanges { target: patientNameInput; visible: false }
                PropertyChanges { target: commentsTextInfo; visible: false }
                PropertyChanges { target: commentsTextArea; visible: false }
                PropertyChanges { target: popupWindow; width: 500; height: 220 }
            },
            State {
                name: "showScanSettings"
                PropertyChanges { target: resultText; visible: true;
                    text: "<p align=\"center\">Scanning completed successfully!</p>" }
                PropertyChanges { target: addScanToPatient; visible: true }
                PropertyChanges { target: saveScanToNewPatient; visible: true }
                PropertyChanges { target: leftScanDescription; visible: true }
                PropertyChanges { target: leftScanId; visible: true }
                PropertyChanges { target: rightScanDescription; visible: true }
                PropertyChanges { target: rightScanId; visible: true }
                PropertyChanges { target: patientNameInfo; visible: true }
                PropertyChanges { target: patientNameInput; visible: true }
                PropertyChanges { target: commentsTextInfo; visible: true }
                PropertyChanges { target: commentsTextArea; visible: true }
                PropertyChanges { target: popupWindow; width: 700; height: 520 }
            }
        ]
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("SeraphRoboticsUi")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }

    Connections {
        target: qmlCppWrapper

        onScanProgressUpdated: {
            progressBar.value = progress
        }

        onScanStarted: {
            console.log("Scan started")
            popupWindow.state = "hideScanSettings"
            progressBar.visible = true
            progressBar.value = 0
            startScanButton.visible = false
        }

        onScanCompleted: {
            console.log("Scan completed")
            popupWindow.state = "showScanSettings"
            progressBar.visible = false
            startScanButton.visible = true
        }

        onScanError: {
            console.log("Scan error")
            resultText.visible = true
            resultText.text = "Error text"
            progressBar.visible = false
            startScanButton.visible = true
            qmlCppWrapper.cleanScanner()
        }
    }
}

