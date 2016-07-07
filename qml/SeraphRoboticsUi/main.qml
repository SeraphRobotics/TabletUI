import QtQuick 2.4
import QtQuick.Dialogs 1.2

import "RegularScreens"
import "RegularScreens/ChoosePatientScreenPrivateComponents" 1.0
import "./Components" 1.0
import "./RegularScreens/SettingsPagePrivateComponents/" 1.0
import "./Components/Overlays"
/*
 Main QtQuick Item which is our root item, here we switch beetwen all screens in applications.
*/
Item {

    // We need to keep starting size due to the canvas issue.
    width: 1280
    height: 800

    Loader {
        id : mainApplicationObject
        objectName : "mainApplicationObject"

        //Value used to keep height for bottom right corner logo element.
        property int logoHeight: 70
        // Store number of initialized canvases
        property int canvasInitializedCount: 0
        property alias loadingOverlay: loadingOverlay

        anchors.fill : parent

        function canvasInitialized() {
            canvasInitializedCount++
            var canvasCount = qmlCppWrapper.isSingleFootMode() ? 1 : 2
            if (canvasInitializedCount === canvasCount) {
                loadingOverlay.hide()
                canvasInitializedCount = 0
            }
        }

        // Page 20-30 from pdf. Item used for showing pads.
        // We don't want to reload page again and again, that's why we keep it created.
        SettingsPageScreen {
            id : settingsPage
            objectName: "settingsPage"

            opacity : mainApplicationObject.state == "settingsPageScreen" ? 1 : 0
            enabled : mainApplicationObject.state == "settingsPageScreen" ? true : false
        }

        LoadingOverlay {
            id: loadingOverlay
            z: 999
        }

        // Page manager (state machine), gets state from stateManager cpp
        // and switches to proper screen.
        Connections {
            target : stateManager

            onSigSwitchToSpecificScreen:
            {
                console.log("From qml page state manager function, got state "+pageName)

                mainApplicationObject.source = ""
                mainApplicationObject.state = pageName

                if(pageName == "summaryPage")
                    mainApplicationObject.source = "InitialSetupWizard/SummaryPage.qml"
                else if(pageName == "saveSettings")
                    mainApplicationObject.source = "InitialSetupWizard/SaveSettingsPage.qml"
                else if(pageName == "noInternetConnection")
                    mainApplicationObject.source = "InitialSetupWizard/NoInternetConnectionPage.qml"
                else if(pageName == "customerNumberPageState")
                    mainApplicationObject.source = "InitialSetupWizard/CustomerNumberPage.qml"
                else if(pageName == "accountSetupState")
                    mainApplicationObject.source = "InitialSetupWizard/AccountSetupPage.qml"
                else if(pageName == "welcomeScreenState")
                    mainApplicationObject.source = "InitialSetupWizard/WelcomePage.qml"
                else if(pageName == "editaccountSetupPage")
                    mainApplicationObject.source = "InitialSetupWizard/EditAccountSetupPage.qml"
                else if(pageName == "editaccountSettingsPage")
                    mainApplicationObject.source = "InitialSetupWizard/EditAccountSettingsPage.qml"
                else if(pageName == "startScreenState")
                    mainApplicationObject.source = "RegularScreens/StartPage.qml"
                else if(pageName == "choosePatientState")
                    mainApplicationObject.source = "RegularScreens/ChoosePatientPage.qml"
                else if(pageName == "patientHistoryScreen")
                    mainApplicationObject.source = "RegularScreens/PatientHistoryPage.qml"
                else if(pageName == "main3dView")
                    mainApplicationObject.source = "RegularScreens/SettingsPagePrivateComponents/ReviewSubpage/Main3dView.qml"
            }
        }

        Component.onCompleted:
        {
            SettingsPageComponentsSettings.m_MainRootObject = mainApplicationObject

            // Set up parent property for singleton popups.
            FramePopup.parent = mainApplicationObject
            TutorialWindowPopup.parent = mainApplicationObject
            GrayPopup.parent = mainApplicationObject
            ScannerPopup.parent = mainApplicationObject
            AddScanToPatientPopup.parent = mainApplicationObject

            console.log("main, onCompleted")
        }
    }

    Connections {
        target: qmlCppWrapper

        onShowAnimationProgress: {
            loadingOverlay.text = text
            loadingOverlay.show();
        }

        onInvalidOrthotic: {
            msgDialog.icon = StandardIcon.Critical
            msgDialog.text = qsTr("Could not load Orthotic with id: %1
                                  \nWe have loaded the appropriate scan file").arg(orthId)
            msgDialog.visible = true
        }

        onScannerDetected: {
            ScannerPopup.setOpacity(1)
        }

        onScannerDisconnected: {
            ScannerPopup.setOpacity(0)
        }
    }

    MessageDialog {
        id: msgDialog
        title: qsTr("SeraphRoboticsUi")
        standardButtons: StandardButton.Ok
    }
}
