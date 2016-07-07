import QtQuick 2.4

Item {
    id : topNavigationPanel

    property alias leftButton: leftButton
    property alias middleButton : middleButton
    property alias rightButton : rightButton
    property alias rightBottomButton : rightBottomButton

    signal leftButtonClicked()
    signal rightButtonClicked()
    signal middleButtonClicked()
    signal rightBottomButtonClicked()

    height : 30

    anchors {
        left : parent.left
        leftMargin: 25

        right : parent.right
        rightMargin : 25

        top : parent.top
        topMargin: 10
    }

    NavigationButton {
        id : leftButton

        anchors {
            top : parent.top
            left : parent.left
        }

        buttonText: qsTr("2: patient history")

        Component.onCompleted: {
            leftOrRightButton("left")
            anchors.topMargin = -8
            anchors.leftMargin = 0
        }

        onButtonClicked: {
            leftButtonClicked()
        }
    }

    NavigationButton {
        id : middleButton

        anchors {
            top : parent.top
        }

        buttonInformationText.font.pixelSize: 25
        buttonText: qsTr("step 3: settings")

        Component.onCompleted: {
            leftOrRightButton("center")
            anchors.horizontalCenter = topNavigationPanel.horizontalCenter
            anchors.horizontalCenterOffset = 40
            anchors.topMargin = -8
        }
        onButtonClicked: {
            middleButtonClicked()
        }
    }
    NavigationButton {
        id : rightButton

        anchors {
            top : parent.top
        }

        buttonText: qsTr("4: review")

        onButtonClicked: {
            rightButtonClicked()
        }

        Component.onCompleted: {
            leftOrRightButton("right")
            anchors.topMargin = -8
            anchors.rightMargin = 0
        }
    }
    NavigationButton {
        id : rightBottomButton

        visible: false

        anchors {
            top : rightButton.bottom
            right: rightButton.right
        }

        buttonText: qsTr("save for later")

        onButtonClicked: {
            rightBottomButtonClicked()
        }

        Component.onCompleted: {
            leftOrRightButton("right")
            anchors.topMargin = 30
            anchors.rightMargin = 0
        }
    }
}
