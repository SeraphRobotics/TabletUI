// View3dPosting.qml

import QtQuick 2.0
import QtCanvas3D 1.0

import "scene_lateral.js" as SceneLateral
import "scene_posterior.js" as ScenePosterior

// 3dView Item - Viewer for stl files in posting section (see page 28).
Column {
    anchors.left: parent.left; anchors.top: parent.top
    spacing: 10

    property string meshSource

    //Lateral view
    Rectangle {
        color: "transparent"
        border.color: "#000000"
        width: parent.width
        height: (parent.height - 3 * spacing - lateralText.height - posteriorText.height) / 2
        radius: 10

        Canvas3D {
            id: sceneLateral
            // need to be able set transparent background
            opacity: 0.99

            anchors.fill: parent
            anchors.margins: 10

            onInitializeGL: SceneLateral.initializeGL(sceneLateral,
                                                      meshSource,
                                                      qmlCppWrapper.currentFoot.scanWidth(),
                                                      qmlCppWrapper.currentFoot.scanHeight())
            onPaintGL: SceneLateral.paintGL(sceneLateral)
            onResizeGL: SceneLateral.resizeGL(sceneLateral)
        }
    }

    Text {
        id: lateralText
        width: parent.width
        horizontalAlignment: Text.AlignHCenter

        text: qsTr("lateral")
    }

    //Posterior view
    Rectangle {
        color: "transparent"
        border.color: "#000000"
        width: parent.width
        height: (parent.height - 3 * spacing - lateralText.height - posteriorText.height) / 2
        radius: 10

        Canvas3D {
            id: scenePosterior
            // need to be able set transparent background
            opacity: 0.99

            anchors.fill: parent
            anchors.margins: 10

            onInitializeGL: ScenePosterior.initializeGL(scenePosterior,
                                                        meshSource,
                                                        qmlCppWrapper.currentFoot.scanWidth(),
                                                        qmlCppWrapper.currentFoot.scanHeight())
            onPaintGL: ScenePosterior.paintGL(scenePosterior)
            onResizeGL: ScenePosterior.resizeGL(scenePosterior)
        }
    }

    Text {
        id: posteriorText
        width: parent.width
        horizontalAlignment: Text.AlignHCenter

        text: qsTr("anterior/posterior")
    }

    onMeshSourceChanged: {
        SceneLateral.setNewStlFile(sceneLateral,
                                   meshSource,
                                   qmlCppWrapper.currentFoot.scanWidth(),
                                   qmlCppWrapper.currentFoot.scanHeight())
        ScenePosterior.setNewStlFile(scenePosterior,
                                     meshSource,
                                     qmlCppWrapper.currentFoot.scanWidth(),
                                     qmlCppWrapper.currentFoot.scanHeight())
    }
}

