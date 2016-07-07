import QtQuick 2.4

import "../../Components"
import "../../Components/Edges"
import "SettingsPageComponents"

Item {
    id : mainWindow

    property alias edgeEditor: edgeEditor

    // This signal is emitted when the user accepted side.
    signal directionSelected()

    anchors
    {
        top : parent.top
        topMargin: 100

        left : parent.left
        leftMargin: 40
        right : parent.right
        bottom : parent.bottom
        bottomMargin: 50
    }

    MouseArea
    {
        anchors.fill: parent
    }

    GroupBoxTemplate {
        id : scanImageBox

        title : qsTr("Right Scan")

        width : parent.width/2-80

        anchors {
            top : parent.top
            bottom : parent.bottom
        }

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
                source : "qrc:/QmlResources/cancel.png"
            }

            ImageBasedButton {
                source : "qrc:/QmlResources/accept.png"

                Connections {
                    target: qmlCppWrapper

                    onHideAnimationProgress: {
                        directionSelected()
                    }
                }

                onCustomClicked:
                {
                    qmlCppWrapper.generateStlModels()
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

            edgeRadius: 15

            onStateChanged:
            {
                if(state === "right")
                {
                    scanImageBox.title = "Right Scan"
                }
                else if(state === "left")
                {
                    scanImageBox.title = "Left Scan"
                }
            }

            Component.onCompleted: {
                state = settingsPageManager.currentSelectedDirection
                initWidth = width
                initHeight = height
            }
        }
    }

    GrayDescriptionText {
        id : descriptionText

        focus : true

        font.pixelSize: 22
        anchors {
            top : scanImageBox.top
            topMargin: scanImageBox.headerHeight
            left : parent.left
            leftMargin: scanImageBox.width+
                        (parent.width-scanImageBox.width)/2-descriptionText.width/2
        }

        text : qsTr("Select the foot by clicking on it.<br/><br/>
Drag the colored dots to define the<br/>
metatarsal edge of the shell.<br/>")
    }

    SettingsPageChooseSideComponent {
        id : selectionArea

        width : mainWindow.width/2-80

        Binding {
             target: selectionArea
             property: "state"
             value: settingsPageManager.currentSelectedDirection !== "both" ?
                        settingsPageManager.currentSelectedDirection : selectionArea.state
         }

        anchors {
            top : descriptionText.bottom
            topMargin: 30
            horizontalCenter: descriptionText.horizontalCenter
            bottom : scanImageBox.bottom
            bottomMargin: 40
        }

        leftSide.width: width * 0.35
        rightSide.width: width * 0.35

        leftSide.height:height* 0.85
        rightSide.height: height* 0.85

        leftSide.view3dRect.visible: false
        rightSide.view3dRect.visible: false

        leftSide.rawImageVisible: true
        rightSide.rawImageVisible: true

        middleTextArea.visible: false

        onStateChanged:
        {
            if(state === "left")
            {
                if(edgeEditor.state != "start")
                    edgeEditor.state = "left"
            }
            else if(state === "right")
            {
                if(edgeEditor.state != "start")
                    edgeEditor.state = "right"
            }
        }
    }

    Connections {
        target: qmlCppWrapper

        onScanImageGenerated: {
            edgeEditor.leftImageSource = "image://scan_images/left"
            edgeEditor.rightImageSource = "image://scan_images/right"
            edgeEditor.initFeet()
            if (selectionArea.state === "left")
                edgeEditor.setLeftEdge()
            else if (selectionArea.state === "right")
                edgeEditor.setRightEdge()

            selectionArea.leftSide.rawImageSource = ""
            selectionArea.rightSide.rawImageSource = ""
            selectionArea.leftSide.rawImageSource = edgeEditor.leftImageSource
            selectionArea.rightSide.rawImageSource = edgeEditor.rightImageSource
        }
    }
}
