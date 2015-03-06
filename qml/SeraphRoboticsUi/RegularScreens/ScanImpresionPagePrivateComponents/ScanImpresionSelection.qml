import QtQuick 2.4
import QtQuick.Dialogs 1.2

import "../../Components"
import "../../Components/Edges"
import "ScanImpresionPageComponents"
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
Item {
    id : root
    // This signal is emitted when the user accepted side.
    signal directionSelected()
    anchors
    {
        fill: parent
        top : parent.top
        topMargin: 100
        left : parent.left
        leftMargin: 40
        right : parent.right
        bottom : parent.bottom
        bottomMargin: 50
    }
    MouseArea {  anchors.fill: parent }
    property  int progressValue: 0

    function scanProgress(step) {
        //console.log("scan progress " + step)
        progressBar.value = step
    }
    function scanError(error) {
        console.log("scan error " + error )
        infoMessage.title = "Scanner error"
        infoMessage.text = error
        infoMessage.open()
        progressBar.value = 0
    }

    Connections { target: qmlCppWrapper; onScanProgress: scanProgress(step)}
    Connections { target: qmlCppWrapper; onScanError: scanError(error)}
    Connections { target: qmlCppWrapper; onScanCompleted:scanStatus.color = "green" }


    MessageDialog {
        id: infoMessage
    }

    Timer {
        id: timer
        interval: 500; running: false; repeat: true
        onTriggered: {
            progressValue += 1
            console.log("progress value " + progressValue)
            progressBar.value = progressValue
            if(progressValue >= 20) {
                progressValue = 0
                stop()
                //restart()
            }
       }
    }


    //GrayDescriptionText {
    Rectangle {
        id: scanRect
        focus: true
        //font.pixelSize: 22
        anchors {
            top : scanImageBox.top
            //horizontalCenter: selectionArea.horizontalCenter
            //topMargin: scanImageBox.headerHeight
            right: selectionArea.right
            left : parent.left
            //leftMargin: scanImageBox.width+
            //            (parent.width-scanImageBox.width)/2-descriptionText.width/2
        }
        Column {
            spacing: 15
            Row {
                spacing: 30
                Text {
                    id: patientNameTextDescription
                    color: "#666666"
                    font.pixelSize: 20
                    //anchors {
                    //    left : parent.left
                        //leftMargin: 70
                        //bottom : prescriberText.top
                        //bottomMargin: 20
                    //}
                    text: "patient name"
                }
                StyledTextInput {
                    id: patientNameInput
                    property real widthScaleValue: 350/1280
                    //width : widthScaleValue*parent.width
                    width: 200
                    horizontalAlignment: Text.AlignLeft
                    deafultFontSize: 14
                    textColor: "#666666"
                    welcomeTextColor : "#666666"
                    readOnly: false
                    text: ""
                    //anchors
                    //{
                     //   verticalCenter: textInputDescription.verticalCenter
                     //   left: textInputDescription.right
                     //   leftMargin : 100
                    //}
                }
            } // Row
            Row {
                id: row1
                spacing: 50
                StyledButton {
                    id : beginScanButton
                    objectName: "beginScanButton"
                    height: 40
                    width: 100
                    text.color: "#ffffff"
                    text.font.pixelSize: 30
                    titleText: "Begin Scan"
                    border.color: "#2e3192"
                    buttonType : "startButton"
                    //anchors {
                    //    top : patientNameTextDescription.bottom
                    //horizontalCenter: parent.horizontalCenter
                    //}
                    onCustomClicked: {
                        qmlCppWrapper.beginScan()
                        scanStatus.color = "red"
                    }
                }
                ProgressBar {
                    id: progressBar
                    clip: true
                    anchors {left: patientNameInput.left }
                    indeterminate: false
                    //value: progressValue
                    minimumValue: 0
                    maximumValue: 334
                    style: ProgressBarStyle {
                        background: Rectangle {
                            //clip: true
                            radius: 20
                            color: "white"
                            border.color:  "#cccccc"
                            border.width: 1
                            implicitWidth: 200
                            implicitHeight: 40
                        }
                        progress: Rectangle {
                            color: "lightsteelblue"
                            radius: 20
                            //border.color: "steelblue"
                        }
                    }
                }
                Rectangle {
                    id: scanStatus
                    height: 30
                    width: 30
                    color: "red"
                    radius: 20
                    anchors.verticalCenter: parent.verticalCenter
                }


            } // Row
        } // Columnn
        //text: "Select the foot by clicking on it.<br/><br/>
        //        Drag the blue dots to define the<br/>
        //        metatarsal edge of the shell.<br/>"
    }

    ScanImpresionChooseSideComponent {
        id: selectionArea
        width: root.width/2-80

        Binding {
             target: selectionArea
             property: "state"
             value: settingsPageManager.currentSelectedDirection !== "both" ?
                        settingsPageManager.currentSelectedDirection : selectionArea.state
         }

        anchors {
            top: scanRect.bottom
            //top: beginScanButton.bottom
            topMargin: 130
            //horizontalCenter: descriptionText.horizontalCenter
            right: scanImageBox.left
            rightMargin: 50
            bottom: scanImageBox.bottom
            bottomMargin: 30
        }

        leftSide.width: width * 0.35
        rightSide.width: width * 0.35

        leftSide.height:height* 0.85
        rightSide.height: height* 0.85

        //leftSide.view3d.visible: true
        //rightSide.view3d.visible: true

        leftSide.rawImageVisible: true
        rightSide.rawImageVisible: true
        middleTextArea.visible: false


        leftSide.rawImageSource:  "qrc:/QmlResources/FootScanPlaceholder.png"
        rightSide.rawImageSource: "qrc:/QmlResources/FootScanPlaceholder.png"
        //leftSide.rawImageSource: "qrc:/exampleImages/left-scan-cut.png"
        //rightSide.rawImageSource: "qrc:/exampleImages/right-scan-cut.png"

        onStateChanged:
        {
            if(state === "needScans") {
                //edgeEditor.imageSource =  "qrc:/exampleImages/right-scan-cut.png"
                //if(edgeEditor.state != "start")
                //    edgeEditor.state = "right"
                edgeEditor.imageSource = ""

            }
            else if(state === "left")
            {
                edgeEditor.imageSource = "qrc:/exampleImages/left-scan-cut.png"
                if(edgeEditor.state != "start")
                    edgeEditor.state = "left"
            }
            else if(state === "right")
            {
                edgeEditor.imageSource =  "qrc:/exampleImages/right-scan-cut.png"
                if(edgeEditor.state != "start")
                    edgeEditor.state = "right"
            }
            // Redraw canvas.
            edgeEditor.canvas.requestPaint()
        }
    }

    GroupBoxTemplate {
        id: scanImageBox
        title: "Right Scan"
        width: parent.width/2-80
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right;  rightMargin: 80 }

        Row {
            id : buttonArea
            spacing : 10
            anchors {
                top : parent.top
                topMargin : 75
                right : parent.right
                rightMargin: 20
            }

            ImageBasedButton {
                source: "qrc:/QmlResources/cancel.png"
                onCustomClicked: {
                    wipDialog.show("cancelScanButton")
                }
            }
            ImageBasedButton {
                source: "qrc:/QmlResources/accept.png"
                onCustomClicked: {
                    wipDialog.show("accectScanButton")
                    //directionSelected()
                }
            }
        }

        // Item used to catch four borders.
        EdgeEditor {
            id: edgeEditor
            state : "start"
            anchors {
                verticalCenter: parent.verticalCenter
                verticalCenterOffset: 10
                horizontalCenterOffset: 0
                horizontalCenter: parent.horizontalCenter
            }
            width: parent.width * 0.5
            height: parent.height * 0.8
            edgeRadius: 25

            onStateChanged:
            {
                if(state === "right")
                {
                    scanImageBox.title =  "Right Scan"
                }
                else if(state === "left")
                {
                    scanImageBox.title =  "Left Scan"
                }
            }

            Component.onCompleted: {
                state = "right"
                initWidth = width
                initHeight = height
            }
        }
    }


}
