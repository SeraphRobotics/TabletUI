import QtQuick 2.4

import "../Components" 1.0
import "." 1.0
import "ScanImpresionPagePrivateComponents/ScanImpresionPageComponents/DraggableElementComponents" 1.0
import "ScanImpresionPagePrivateComponents"
//import "SettingsPagePrivateComponents/SettingsPageComponents/DraggableElementComponents" 1.0
import "SettingsPagePrivateComponents" 1.0
import "../"

PageTemplate {
    id : root
    anchors.fill: parent
    property int extensibleAreaUpHeight : 30
    property int extensibleAreaSpaceBeetwen : 15
    state : "selection"

    WipDialog { id: wipDialog }

    // Top navigation panel
    NavigationBarWithButtons
    {
        id : topNavigationPanel
        z : currentItemUnselectArea.z+1
        leftButton.buttonText: "Delete Rx and\nReturn to Start"
        middleButton.buttonText: "Scan an Impresion"
        rightButton.buttonText: "Create Rx\nfor Production"

        onLeftButtonClicked:
        {
            stateManager.setState("startScreenState")
            /*
            /// Unselect pad if any selected.
            SettingsPageComponentsSettings.unselectCurrentDraggablePad()
            //  If the state is 3dReview load last modificationPage.state
            if(modificationPage.state === "3dReview")
            {
                modificationPage.state = modificationPage.earlierState
                mainScreen.state = "selection"
            }
            else
            {
                /// Restore shellModifications view to normal state - list with pad.
                if(settingsPageManager.shellModificationsState != "up")
                    settingsPageManager.shellModificationsState = "down"
                stateManager.setState("patientHistoryScreen")
            }
            */
        }
        onRightButtonClicked:
        {
            wipDialog.show("nav create rx for production")
            /*
            SettingsPageComponentsSettings.unselectCurrentDraggablePad()
            if(modificationPage.state === "3dReview")
            {
                SettingsPageComponentsSettings.detectBordersAndSaveToXml()
            }

            mainScreen.state = "modification"
            modificationPage.state = "3dReview"
            */
        }
        onMiddleButtonClicked:
        {
            SettingsPageComponentsSettings.unselectCurrentDraggablePad()
            TutorialWindowPopup.showPopup()
        }
    }

    // use in SettingsPageModificationScreen 3dReview state.
    // Middle gray button from page 30.
    StyledButton
    {
        id : viewButton
        width : 100
        titleText : "3D view"
        visible : true
        anchors {
            horizontalCenter:  parent.horizontalCenter
            top : parent.top
            topMargin: 50
        }
        onCustomClicked: stateManager.setState("main3dView")
    }

    UnselectAreaForDraggableElements
    {
        id : currentItemUnselectArea
    }

    // selecte current foot and selected four border corners.
    ScanImpresionSelection {
        id : selectPage
        //opacity : 1
        z : currentItemUnselectArea.z+1
        // Signal emmited when user confirmed direction. Now we can switch from page 19 to page 20 pdf.
        onDirectionSelected:
        {
            wipDialog.show("farts")
            //console.log("User selected direction go to: "+modificationPage.earlierState)
            //root.state = "modification"
            //modificationPage.state = modificationPage.earlierState
        }
    }

    // Item used to show 20-30 pages logic.
    /*
    SettingsPageModificationScreen
    {
        id : modificationPage
        z : currentItemUnselectArea.z+1
    }
    */

    states: [
        State {
            name: "selection"
            //PropertyChanges { target: modificationPage; opacity : 0 }
            //PropertyChanges { target: modificationPage; z : -99 }
            //PropertyChanges { target: selectPage; opacity : 1 }
        },
        State {
            name: "modification"
            PropertyChanges { target: selectPage
                opacity : 0}
            PropertyChanges { target: modificationPage
                opacity : 1}
            //PropertyChanges { target: topNavigationPanel.leftButton
            //    buttonText : "2: patient history"}
            //PropertyChanges { target: topNavigationPanel.middleButton
            //    buttonText : "step 3: settings"}
            //PropertyChanges { target: topNavigationPanel.rightButton
            //    buttonText : "4: review"}
        }
    ]

    onOpacityChanged:
    {
        if(opacity === 1 && modificationPage.state !== "3dReview")
            state = "selection"
    }

    onLogoButtonClicked:
    {
        SettingsPageComponentsSettings.unselectCurrentDraggablePad()
    }

    onStateChanged:
    {
        //settingsPageManager.mainSettingsPageState = state
    }
}







