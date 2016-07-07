import QtQuick 2.4

import "../../../Components"
import "../SettingsPageComponents"

SettingsPageExtensibleArea {
    id : posting

    index : 0
    title : qsTr("Posting")
    property alias root : root

    // return posting values for foot (left or right)
    function getPostingValues(foot) {
        var upBox, downBox
        if (foot === "left") {
            upBox = leftUpBox
            downBox = leftDownBox
        } else if (foot === "right") {
            upBox = rightUpBox
            downBox = rightDownBox
        }

        var angleHeal = upBox.varusVal
        var verticleHeal = upBox.verticleVal
        var varusDirectionHeal = upBox.varusDirection
        var angleFore = downBox.varusVal
        var verticleFore = downBox.verticleVal
        var varusDirectionFore = downBox.varusDirection
        return [angleHeal, verticleHeal, varusDirectionHeal,
                angleFore, verticleFore, varusDirectionFore];
    }

    // set posting values for foot (left or right)
    function setPostingValues(foot, posting) {
        var upBox, downBox
        if (foot === "left") {
            upBox = leftUpBox
            downBox = leftDownBox
        } else if (foot === "right") {
            upBox = rightUpBox
            downBox = rightDownBox
        }
        upBox.varusVal = posting[0]
        upBox.verticleVal = posting[1]
        upBox.setDirection(posting[2])
        downBox.varusVal = posting[3]
        downBox.verticleVal = posting[4]
        downBox.setDirection(posting[5])
    }

    // set default posting values for both feet
    function setDefaultValues() {
        leftUpBox.varusVal = 0
        leftUpBox.verticleVal = 0
        leftUpBox.setDirection(1)
        leftDownBox.varusVal = 0
        leftDownBox.verticleVal = 0
        leftDownBox.setDirection(1)
        rightUpBox.varusVal = 0
        rightUpBox.verticleVal = 0
        rightUpBox.setDirection(1)
        rightDownBox.varusVal = 0
        rightDownBox.verticleVal = 0
        rightDownBox.setDirection(1)
    }

    Binding {
         target: root
         property: "state"
         value: settingsPageManager.currentSelectedDirection !== "both" ?
                    settingsPageManager.currentSelectedDirection : root.state
     }

    Item {
        id : root

        width : parent.width
        height : parent.height

        clip : true
        state: "left"

        anchors {
            top : parent.top
            topMargin: 70
            left : parent.left
            leftMargin: 0
        }

        //Axis Lines
        Rectangle {
            id: horizontalLine

            color: "black"
            x: parent.width / 5
            anchors {
                top : parent.top
                topMargin:  parent.height / 2
            }

            clip : true
            width: parent.width - x * 1.4; height: 2
        }

        Rectangle {
            id:verticalLine

            color: "black"
            width: 2;

            anchors{
                top : parent.top
                topMargin: 100
                bottom : parent.bottom
                bottomMargin : 200
            }

            Behavior on x { PropertyAnimation {} }
            clip : true
        }

        //Right and Left foot modes switching Rectangles
        Rectangle {
            id: rFootRect

            property alias textCol: rFootRectText.color

            color: "transparent"
            anchors.bottom: leftUpBox.top
            anchors.margins: 70
            width: horizontalLine.width / 5 * 1.5
            height: width / 4
            radius: height / 5
            clip : true

            x: horizontalLine.x + ((verticalLine.x - horizontalLine.x) /2 ) - (width / 2)

            Text {
                id: rFootRectText

                color: "#666666"

                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: height / 1.5
                text: qsTr("Right Foot")
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                        settingsPageManager.currentSelectedDirection = "right"
                }
            }

            Behavior on x { PropertyAnimation{ } }
            Behavior on border.color {
                ColorAnimation { }
            }
        }

        //Left
        Rectangle {
            id: lFootRect

            property alias textCol: lFootRectText.color

            color: "transparent"
            anchors.bottom: leftUpBox.top
            anchors.margins: 70
            clip : true
            width: horizontalLine.width / 5 * 1.5
            height: width / 4
            radius: height / 5

            x: verticalLine.x +
               ((horizontalLine.x + horizontalLine.width - verticalLine.x) / 2) - (width / 2)

            Text {
                id: lFootRectText

                color: "#666666"

                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: height / 1.5
                text: qsTr("Left Foot")
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                        settingsPageManager.currentSelectedDirection = "left"
                }
            }

            Behavior on x { PropertyAnimation{ } }

            Behavior on border.color {
                ColorAnimation { }
            }
        }

        //Invisible helping border lines
        Rectangle {
            id: leftBorder

            clip : true
            color: "transparent"
            x: horizontalLine.x
            y: horizontalLine.y - (verticalLine.height / 2 )
            width: 1; height: verticalLine.height
        }

        Rectangle {
            id: rightBorder

            clip : true
            color: "transparent"
            x: horizontalLine.x + horizontalLine.width
            y: horizontalLine.y - (verticalLine.height / 2 )
            width: 1; height: verticalLine.height
        }

        //"Rearfoot" and "Forefoot" text Rectangles
        Rectangle {
            id: rearFootRect

            color: "transparent"
            clip : true
            anchors.left: parent.left
            anchors.right: leftBorder.left
            anchors.verticalCenter: leftUpBox.verticalCenter
            height: rFootRect.height

            Text {
                color: "#666666"

                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: height / 1.5
                text: qsTr("Rearfoot")
            }
        }

        Rectangle {
            id: foreFootRect

            color: "transparent"
            clip : true
            anchors.left: parent.left
            anchors.right: leftBorder.left
            anchors.verticalCenter: leftDownBox.verticalCenter
            height: rFootRect.height

            Text {
                color: "#666666"

                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: height / 1.5
                text: qsTr("Forefoot")
            }
        }

        //Main Boxes
        //values is: verticleVal and varusVal
        PostingBox {
            id: leftUpBox

            clip : true
            anchors.left: leftBorder.right
            anchors.bottom: horizontalLine.top
            height: verticalLine.height / 2
            width: horizontalLine.width / 5 * 3
        }

        PostingBox {
            id: rightUpBox

            clip : true
            anchors.left: verticalLine.left
            anchors.bottom: horizontalLine.top
            height: verticalLine.height / 2
            width: horizontalLine.width / 5 * 3
        }

        PostingBox {
            id: leftDownBox

            clip : true
            anchors.left: leftBorder.right
            anchors.top: horizontalLine.bottom
            height: verticalLine.height / 2
            width: horizontalLine.width / 5 * 3
        }

        PostingBox {
            id: rightDownBox

            clip : true
            anchors.left: verticalLine.left
            anchors.top: horizontalLine.bottom
            height: verticalLine.height / 2
            width: horizontalLine.width / 5 * 3
        }

        states: [
            State {
                name: "right"
                extend: "down"
                PropertyChanges { target: verticalLine;
                    x: horizontalLine.x + (horizontalLine.width / 5 *3) }
                PropertyChanges {
                    target: rFootRect;
                    border.color: "#588EE6"
                    textCol: "#666666"
                }
                PropertyChanges {
                    target: lFootRect;
                    border.color: "transparent"
                    textCol: "#B3B3B3"
                }

                PropertyChanges { target: leftUpBox; state: "activeState" }
                PropertyChanges { target: leftDownBox; state: "activeState" }
                PropertyChanges { target: rightUpBox; state: "showingState" }
                PropertyChanges { target: rightDownBox; state: "showingState" }
            },
            State {
                name: "left"
                extend: "down"
                PropertyChanges { target: verticalLine; x: horizontalLine.x + (horizontalLine.width / 5 *2) }
                PropertyChanges {
                    target: rFootRect;
                    border.color: "transparent"
                    textCol: "#B3B3B3"
                }
                PropertyChanges {
                    target: lFootRect;
                    border.color: "#588EE6"
                    textCol: "#666666"
                }

                PropertyChanges { target: leftUpBox; state: "showingState" }
                PropertyChanges { target: leftDownBox; state: "showingState" }
                PropertyChanges { target: rightUpBox; state: "activeState" }
                PropertyChanges { target: rightDownBox; state: "activeState" }
            }
        ]

        onHeightChanged: {
            if(height < 250)
                root.visible = false
            else if(height > 250 && root.visible == false)
                root.visible = true
        }
    }
}

