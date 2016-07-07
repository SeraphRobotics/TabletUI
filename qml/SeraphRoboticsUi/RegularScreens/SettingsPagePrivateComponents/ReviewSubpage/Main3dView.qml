import QtQuick 2.4
import QtQuick.Controls 1.4
import "../../../"
import "../../../Components"
import "../" 1.0
import "../SettingsPageComponents/dynamicobjectcreation.js" as ObjectCreationScript

PageTemplate {

    function loadModels(selector) {
        console.log("loading 3d preview for " + selector + " model(s)")
        var left, right;
        if (selector === "left") {
            left = qmlCppWrapper.leftFoot.stlModelFile()
            right = ""
            qmlCppWrapper.preview3d.setAreaSize( Qt.size( qmlCppWrapper.leftFoot.scanHeight(),
                                                          qmlCppWrapper.leftFoot.scanWidth()))
        }
        else if (selector === "right") {
            left = ""
            right = qmlCppWrapper.rightFoot.stlModelFile()
            qmlCppWrapper.preview3d.setAreaSize( Qt.size( qmlCppWrapper.rightFoot.scanHeight(),
                                                          qmlCppWrapper.rightFoot.scanWidth()))
        }
        else if (selector === "both") {
            left = qmlCppWrapper.leftFoot.stlModelFile()
            right = qmlCppWrapper.rightFoot.stlModelFile()
            qmlCppWrapper.preview3d.setAreaSize( Qt.size( Math.max(qmlCppWrapper.leftFoot.scanHeight(),
                                                                   qmlCppWrapper.rightFoot.scanHeight()),
                                                          qmlCppWrapper.leftFoot.scanWidth() +
                                                          qmlCppWrapper.rightFoot.scanWidth()))
        }
        var mainAppWindow = SettingsPageComponentsSettings.m_MainRootObject;
        qmlCppWrapper.preview3d.setSceneSize(Qt.size(view3d.width, view3d.height));
        qmlCppWrapper.preview3d.setWindowSize(Qt.size(mainAppWindow.width, mainAppWindow.height));
        qmlCppWrapper.preview3d.applyChanges()
        ObjectCreationScript.createPerspective3d(left, right, view3dRect, bindRotationPanel)
    }

    function bindRotationPanel( view ) {
        //view should be of View3dperspective type
        view.modelRotation = Qt.binding(function() {return Qt.vector3d(sliderX.value, sliderY.value, sliderZ.value)})
    }

    Component.onCompleted: loadModels( settingsPageManager.currentSelectedDirection )

    ImageBasedButton {
        width : sourceSize.width
        height : sourceSize.height

        smooth: true
        anchors {
            right : parent.right
            rightMargin : 20
            top : parent.top
            topMargin: 20
        }

        onCustomClicked:
        {
            stateManager.setState(stateManager.previousState)
        }
    }

    Rectangle {
        id: view3dRect

        anchors.centerIn: parent
        width: parent.width * 0.8
        height: parent.height * 0.8

        radius: width * 0.01

        border.color: "black"
        border.width: 2

        color: "transparent"

        View3dPerspective {
            id: view3d
            anchors.fill: parent
            anchors.margins: parent.border.width
        }
    }
    Row {
        anchors.horizontalCenter: view3dRect.horizontalCenter
        anchors.bottom: view3dRect.bottom
        anchors.margins: view3dRect.border.width
        spacing: view3dRect.border.width
        Text {text: qsTr("x: ") + sliderX.value; horizontalAlignment: Text.Right; width: 40}
        Slider {id: sliderX; width: 200; value: 0; stepSize: 1; minimumValue: -90; maximumValue: 90}
        Text {text: qsTr("y: ") + sliderY.value; horizontalAlignment: Text.Right; width: 40}
        Slider {id: sliderY; width: 200; value: 0; stepSize: 1; minimumValue: -90; maximumValue: 90}
        Text {text: qsTr("z: ") + sliderZ.value; horizontalAlignment: Text.Right; width: 40}
        Slider {id: sliderZ; width: 200; value: 0; stepSize: 1; minimumValue: -90; maximumValue: 90}
    }

    Text {
        anchors.horizontalCenter: view3dRect.horizontalCenter
        anchors.bottom: view3dRect.top
        anchors.top: parent.top
        width: view3dRect.width / 3
        color: "black"
        font.pointSize: 14
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignBottom
        text: qsTr("3D Render View")
    }
}
