// View3d.qml

import QtQuick 2.0
import QtCanvas3D 1.0

import "../../RegularScreens/SettingsPagePrivateComponents/" 1.0

import "scene.js" as Scene

// 3dView Item - Viewer for stl files.

Canvas3D {
    property string meshSource
    property string footSide // leftSide/rightSide

    signal componentInitialized()

    function setStlFile(source) {
        var foot
        if (footSide == "leftSide")
            foot = qmlCppWrapper.leftFoot
        else if (footSide == "rightSide")
            foot = qmlCppWrapper.rightFoot

        Scene.setNewStlFile(view1,
                            source,
                            foot.scanWidth(),
                            foot.scanHeight())
    }

    id: view1
    // need to be able set transparent background
    opacity: 0.99

    onInitializeGL: {
        var foot
        if (footSide == "leftSide")
            foot = qmlCppWrapper.leftFoot
        else if (footSide == "rightSide")
            foot = qmlCppWrapper.rightFoot

        Scene.initializeGL(view1,
                           meshSource,
                           foot.scanWidth(),
                           foot.scanHeight())
    }
    onPaintGL: Scene.paintGL(view1)
    onResizeGL: Scene.resizeGL(view1)

    onComponentInitialized: {
        SettingsPageComponentsSettings.m_MainRootObject.canvasInitialized()
    }
}
