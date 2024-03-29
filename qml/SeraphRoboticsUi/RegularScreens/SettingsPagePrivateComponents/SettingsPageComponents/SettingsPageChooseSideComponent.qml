import QtQuick 2.4

import "../../../Components"
import "DraggableElementComponents" 1.0
import "../" 1.0

Item {
    id : chooseComponent

    property alias leftSide : leftSide
    property alias rightSide : rightSide

    property alias middleTextArea : middleTextArea

    property int spacing : 0

    property real widthScaleValue: 650/1280
    property real heightScaleValue: 700/800

    signal showTopcoatApplySettingsPopup()

    width : widthScaleValue*parent.width
    height : heightScaleValue*parent.height

    SettingsPageChooseSideElement {
        id : leftSide
        objectName: "leftSide"

        anchors {
            right : middleTextArea.left
            rightMargin: spacing
        }

        onElementClicked:
        {
            // disable changing foot if we have only 1 foot
            if (qmlCppWrapper.isSingleFootMode())
                return

            SettingsPageComponentsSettings.unselectCurrentDraggablePad()
            settingsPageManager.currentSelectedDirection = "left"
        }
    }

    ChooseSideElement {
        id : middleTextArea

        textElement.text: qsTr("both")

        anchors {
            horizontalCenter: parent.horizontalCenter
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                // disable changing foot if we have only 1 foot
                if (qmlCppWrapper.isSingleFootMode())
                    return

                SettingsPageComponentsSettings.unselectCurrentDraggablePad()
                if(settingsPageManager.shellModificationsState === "padCreator")
                {
                    console.log("Not able to draw on two elements in the same time")
                    return
                }

                if(settingsPageManager.currentSelectedDirection != "both" &&
                             settingsPageManager.modificationState === "topcoat")
                {
                    showTopcoatApplySettingsPopup()
                }

                else
                {
                    settingsPageManager.currentSelectedDirection = "both"
                }
            }
        }
    }

    SettingsPageChooseSideElement {
        id : rightSide
        objectName: "rightSide"

        position.text: qsTr("right")

        anchors {
            left : middleTextArea.right
            leftMargin: spacing
        }

        onElementClicked: {
            // disable changing foot if we have only 1 foot
            if (qmlCppWrapper.isSingleFootMode())
                return

            SettingsPageComponentsSettings.unselectCurrentDraggablePad() /// Selected draggable element lost focus so we need to unselect this element
            settingsPageManager.currentSelectedDirection = "right"
        }
    }

    states : [
        State {
            name : "both"
            PropertyChanges { target: leftSide;   state:  "select"}
            PropertyChanges { target: rightSide;  state:  "select"}
            PropertyChanges { target: leftSide.textContainer;  border.color:  "transparent"}
            PropertyChanges { target: rightSide.textContainer; border.color:  "transparent"}
            PropertyChanges { target: middleTextArea;  border.color:  "#407ee2"}
        },
        State {
            name : "left"
            PropertyChanges { target: leftSide.textContainer;  border.color:  "#407ee2"}
            PropertyChanges { target: leftSide;   state:  "select"}
            PropertyChanges { target: rightSide;  state:  "unselect"}
        },
        State {
            name : "right"
            PropertyChanges { target: rightSide.textContainer; border.color:  "#407ee2"}
            PropertyChanges { target: leftSide;   state:  "unselect"}
            PropertyChanges { target: rightSide;  state:  "select"}
        }
    ]

}
