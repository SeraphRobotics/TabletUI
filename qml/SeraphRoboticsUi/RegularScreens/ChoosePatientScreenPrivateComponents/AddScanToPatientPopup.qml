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

        width: 700
        height: 480

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
                    background.opacity = 0
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

            text : qsTr("<p align=\"center\">You can add the scan(s) to the current patient or create the new patient from the scan(s)</p>")
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
                    id : addScanToPatient

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

                    width : 220
                    text : qsTr("foot scan id")
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                StyledTextInput {
                    id : leftScanId

                    width : 220
                    textColor: "#666666"
                    welcomeTextColor : "#666666"
                }

                GrayDescriptionText {
                    id : rightScanDescription

                    width : 220
                    text : qsTr("another foot scan id")
                    font.pixelSize: 15
                    horizontalAlignment: Text.AlignHCenter
                }

                StyledTextInput {
                    id : rightScanId

                    width : 220

                    textColor: "#666666"
                    welcomeTextColor : "#666666"
                }

            }

            Column {
                spacing : 15

                GrayDescriptionText {
                    id : patientNameInfo

                    text : qsTr("patient name")
                    font.pixelSize: 20
                }

                StyledTextInput {
                    id : patientNameInput

                    textColor: "#666666"
                    welcomeTextColor : "#666666"

                    anchors.right : parent.right
                    anchors.left : parent.left
                }

                GrayDescriptionText {
                    id : commentsTextInfo

                    text : qsTr("comments")
                    font.pixelSize: 20
                }

                CommentsTextArea {
                    id: commentsTextArea
                }
            }
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("SeraphRoboticsUi")
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Ok
    }
}

