import QtQuick 2.4
import QtQuick.Dialogs 1.2

import "../Components" 1.0
import "." 1.0
import "SettingsPagePrivateComponents/SettingsPageComponents/DraggableElementComponents" 1.0
import "SettingsPagePrivateComponents"
import "../"
import ManipulationData 1.0

// Step 3 - Settings Screen - Have two states :
// selection - Page 19.
// modification - Page (20,30).
PageTemplate {
    id : mainScreen

    property int extensibleAreaUpHeight : 30
    property int extensibleAreaSpaceBeetwen : 15
    state : "selection"

    function saveUserChanges()
    {
        var leftPosting = modificationPage.posting.getPostingValues("left")
        var rightPosting = modificationPage.posting.getPostingValues("right")
        var leftTopcoat = modificationPage.topcoat.getTopcoatValues("left")
        var rightTopcoat = modificationPage.topcoat.getTopcoatValues("right")
        qmlCppWrapper.leftFoot.applyUserChanges(leftPosting, leftTopcoat)
        qmlCppWrapper.rightFoot.applyUserChanges(rightPosting, rightTopcoat)
        qmlCppWrapper.generateStlModels()

        // set new page after saving changes
        modificationPage.earlierState = modificationPage.state
        modificationPage.state = "3dReview"
    }

    function applyOrthoSettings()
    {
        var orthoticSettings = qmlCppWrapper.getOrthoticSettings()

        var view3dSize
        if (qmlCppWrapper.currentFoot === qmlCppWrapper.leftFoot) {
            // apply posting settings
            modificationPage.posting.setPostingValues("left", orthoticSettings.slice(0, 5))
            // apply topcoat settings
            modificationPage.topcoat.setTopcoatValues("left", orthoticSettings.slice(6))
            view3dSize = modificationPage.leftSide.getView3dSize()
        } else {
            // apply posting settings
            modificationPage.posting.setPostingValues("right", orthoticSettings.slice(0, 5))
            // apply topcoat settings
            modificationPage.topcoat.setTopcoatValues("right", orthoticSettings.slice(6))
            view3dSize = modificationPage.rightSide.getView3dSize()
        }

        // apply modifications settings
        var modifications = qmlCppWrapper.getModifications(view3dSize)
        for (var i = 0; i < modifications.length; ++i) {
            var modelItem = modificationPage.shellModifications
                .draggableElementList.model.getElementByType(modifications[i].type)
            var location = modifications[i].location;
            SettingsPageComponentsSettings.createObject(modelItem.svgItem,
                                                        modelItem.name,
                                                        modifications[i].type,
                                                        "restore",
                                                        undefined,
                                                        undefined,
                                                        location.x,
                                                        location.y,
                                                        0)
            SettingsPageComponentsSettings.draggableObjectArray
                    [SettingsPageComponentsSettings.
                     draggableObjectArray.length - 1].setItemAsCurrentItem()
            SettingsPageComponentsSettings.saveValues(modifications[i].stiffness,
                                                      modifications[i].depth+modifications[i].thickness,
                                                      modifications[i].depth)
            modificationPage.shellModifications.elementSettings.hide()
        }
    }

    // Top navigation panel
    NavigationBarWithButtons
    {
        id : topNavigationPanel

        z : currentItemUnselectArea.z+2

        onLeftButtonClicked:
        {
            /// Unselect pad if any selected.
            SettingsPageComponentsSettings.unselectCurrentDraggablePad()

            //  If the state is 3dReview load last modificationPage.state
            if (modificationPage.state === "3dReview")
            {
                if (modificationPage.earlierState === "")
                {
                    mainScreen.state = "selection"
                    modificationPage.state = "shellModifications"
                }
                else
                {
                    mainScreen.state = "modification"
                    modificationPage.state = modificationPage.earlierState
                }
            }
            else if (mainScreen.state === "modification")
            {
                SettingsPageComponentsSettings.removeAllPads()
                qmlCppWrapper.cleanFeetModifications()
                mainScreen.state = "selection"
                modificationPage.state = "shellModifications"
            }
            else
            {
                /// Restore shellModifications view to normal state - list with pad.
                if(settingsPageManager.shellModificationsState != "up")
                    settingsPageManager.shellModificationsState = "down"
                stateManager.setState("patientHistoryScreen")
                modificationPage.leftSide.rawImageSource = ""
                modificationPage.rightSide.rawImageSource = ""

                modificationPage.posting.setDefaultValues()
                modificationPage.topcoat.setDefaultValues()
                modificationPage.state = "shellModifications"
            }
        }
        onRightButtonClicked:
        {
            if (mainScreen.state === "selection" && modificationPage.state !== "3dReview") {
                // current page is selectPage we should go to 3dReview page
                qmlCppWrapper.generateStlModels()
                modificationPage.earlierState = ""
                modificationPage.state = "3dReview"
                return
            }

            SettingsPageComponentsSettings.unselectCurrentDraggablePad()

            if (modificationPage.state === "shellModifications" ||
                    modificationPage.state === "topcoat" ||
                    modificationPage.state === "posting") {
                // add all modifications if we have them
                // it's async operation so we should waiting for finished signal
                if (SettingsPageComponentsSettings.draggableObjectArray.length !== 0)
                    SettingsPageComponentsSettings.detectBordersAndSaveToXml()
                else
                    saveUserChanges()
            } else if (modificationPage.state === "3dReview") {
                if (qmlCppWrapper.usbDetected) {
                    // generate and save GCode
                    qmlCppWrapper.generateGcode()
                } else {
                    messageDialog.icon = StandardIcon.Warning
                    messageDialog.text = qsTr("Please, insert flash drive to be able to save GCode!")
                    messageDialog.visible = true
                    messageDialog.calculateImplicitWidth()
                }
            }
        }
        onRightBottomButtonClicked:
        {
            var patient = patientsListModel.getSpecificItem(patientsListModel.currentIndex)
            // generate and save *.ortho file
            qmlCppWrapper.saveOrtho(patient.id, patient.doctorId)
            messageDialog.icon = StandardIcon.Information
            messageDialog.text = qsTr("Orthotic file was saved successfully!")
            messageDialog.visible = true
            messageDialog.calculateImplicitWidth()
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
        titleText : qsTr("3D view")

        visible : false

        anchors {
            horizontalCenter:  parent.horizontalCenter
            top : parent.top
            topMargin: 50
        }

        onCustomClicked:
        {
            stateManager.setState("main3dView")
        }
    }

    UnselectAreaForDraggableElements
    {
        id : currentItemUnselectArea
    }

    // Page 19 from pdf using for selected current foot and selected  four border corners.
    SettingsPageSelectScreen {
        id : selectPage

        opacity : 1
        z : currentItemUnselectArea.z+1

        // Signal emmited when user confirmed direction. Now we can switch from page 19 to page 20 pdf.
        onDirectionSelected:
        {
            console.log("User selected direction go to: "+modificationPage.earlierState)
            if (modificationPage.state === "shellModifications")
                mainScreen.state = "modification"

            if (qmlCppWrapper.isSingleFootMode()) {
                if (qmlCppWrapper.currentFoot === qmlCppWrapper.leftFoot)
                    modificationPage.leftSide.setNewScene(qmlCppWrapper.leftFoot.stlModelFile())
                else
                    modificationPage.rightSide.setNewScene(qmlCppWrapper.rightFoot.stlModelFile())
            } else {
                modificationPage.leftSide.setNewScene(qmlCppWrapper.leftFoot.stlModelFile())
                modificationPage.rightSide.setNewScene(qmlCppWrapper.rightFoot.stlModelFile())
            }

            // apply all modifications if they are in ortho
            if (modificationPage.state === "shellModifications"
                    && qmlCppWrapper.useOrthoFile())
            {
                applyOrthoSettings()
            }
        }
    }

    // Item used to show 20-30 pages logic.
    SettingsPageModificationScreen
    {
        id : modificationPage

        z : currentItemUnselectArea.z+1
    }

    states: [
        State {
            name: "selection"
            PropertyChanges { target: modificationPage; opacity : 0 }
            PropertyChanges { target: modificationPage; z : 0 }
            PropertyChanges { target: selectPage; opacity : 1 }
        },
        State {
            name: "modification"
            PropertyChanges { target: selectPage;
                opacity : 0}
            PropertyChanges { target: modificationPage;
                opacity : 1}
            PropertyChanges { target: topNavigationPanel.leftButton;
                buttonText : qsTr("3: settings")}
            PropertyChanges { target: topNavigationPanel.middleButton;
                buttonText : qsTr("step 3: modifications")}
            PropertyChanges { target: topNavigationPanel.rightButton;
                buttonText : qsTr("4: review")}
            PropertyChanges { target: topNavigationPanel.rightBottomButton;
                visible : false }
            PropertyChanges { target: viewButton;  visible : false }
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
        settingsPageManager.mainSettingsPageState = state
    }

    Connections {
        target: qmlCppWrapper

        onGenerateGcodeFinished: {
            SettingsPageComponentsSettings.m_MainRootObject.loadingOverlay.hide()
            messageDialog.icon = StandardIcon.Information
            messageDialog.text = qsTr("GCode was generated successfully!")
            messageDialog.visible = true
            messageDialog.calculateImplicitWidth()
        }

        onGenerateGcodeFailed: {
            SettingsPageComponentsSettings.m_MainRootObject.loadingOverlay.hide()
            messageDialog.icon = StandardIcon.Critical
            messageDialog.text = qsTr("GCode generation failed! ") + error
            messageDialog.visible = true
            messageDialog.calculateImplicitWidth()
        }
    }

    Connections {
        target : SettingsPageComponentsSettings

        onModificationsWereAdded: {
            saveUserChanges()
        }
    }

    MessageDialog {
        id: messageDialog
        title: qsTr("SeraphRoboticsUi")
        standardButtons: StandardButton.Ok
    }
}
