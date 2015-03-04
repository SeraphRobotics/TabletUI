import QtQuick 2.4

import ".."
import "../Components"
import "StartScreenPrivateComponents"

/*
    Main Ui screen page 12 from complete2June14.pdf
*/
PageTemplate {
    id : startScreen
    anchors.fill: parent

    StyledButton {
        id : uploadToSDRxButton
        height : 80
        anchors.horizontalCenterOffset: -344

        text.color: "#ffffff"
        text.font.pixelSize: 30
        titleText: "Upload Rx to SD"

        border.color: "#2e3192"

        buttonType : "startButton"
        anchors {
            top : parent.top
            topMargin: 496
            horizontalCenter: parent.horizontalCenter
        }
        onCustomClicked:
        {
            console.log("upload to sd button pushed")
        }
    }

    StyledButton {
        id : startNewRxButton
        height : 80
        anchors.horizontalCenterOffset: 0

        text.color: "#ffffff"
        text.font.pixelSize: 30
        titleText: "Start New Rx"

        border.color: "#2e3192"

        buttonType : "startButton"
        anchors {
            top : parent.top
            topMargin: 496
            horizontalCenter: parent.horizontalCenter
        }
        onCustomClicked:
        {
            console.log("Start new rx button pushed")
            //startScreen.state = "pinInputShowing"
        }
    }

    StyledButton {
        id : deleteAllFilesButton
        height : 80
        anchors.horizontalCenterOffset: 350

        text.color: "#ffffff"
        text.font.pixelSize: 30
        titleText: "Delete all files"

        border.color: "#2e3192"

        buttonType : "startButton"
        anchors {
            top : parent.top
            topMargin: 496
            horizontalCenter: parent.horizontalCenter
        }
        onCustomClicked:
        {
            console.log("delete all files button pushed")
        }
    }

    StartScreenBottomPanel {
        //rootVisibleElement : startScreen
        anchors {
            bottom : parent.bottom
            bottomMargin : 15
            horizontalCenter: parent.horizontalCenter
        }
    }

}
