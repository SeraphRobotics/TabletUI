import QtQuick 2.4
import QtQuick.Scene3D 2.0
import Qt3D 2.0
import Qt3D.Renderer 2.0

Scene3D {
    property alias leftSource:  loaderLeft.source
    property alias rightSource: loaderRight.source
    property vector3d modelRotation: Qt.vector3d(0,0,0)

    Entity { id: rootEntity
        property bool isOneModel: leftSource == "" || rightSource == ""

        components: FrameGraph {
            activeFrameGraph: ForwardRenderer {
                clearColor: "#4561eb"
                camera: camera
            }
        }

        Camera {
            id: camera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: qmlCppWrapper.preview3d.cameraAngle
            aspectRatio: qmlCppWrapper.preview3d.cameraRatio
            nearPlane : 1
            farPlane : 1000
            upVector: Qt.vector3d(1.0, 0.0, 0.0)
            position: Qt.vector3d(qmlCppWrapper.preview3d.cameraShift.x,
                                  qmlCppWrapper.preview3d.cameraShift.y,
                                  qmlCppWrapper.preview3d.cameraHeight)
            viewCenter: Qt.vector3d(qmlCppWrapper.preview3d.cameraShift.x,
                                    qmlCppWrapper.preview3d.cameraShift.y, 0.0)
        }

        //Left orthotic
        Transform { id: leftTransform
            Translate { translation: rootEntity.isOneModel ?
                                     Qt.vector3d(-0.5 * qmlCppWrapper.leftFoot.scanHeight(),
                                                 -0.5 * qmlCppWrapper.leftFoot.scanWidth(), 0) :
                                     Qt.vector3d(-0.5 * qmlCppWrapper.leftFoot.scanHeight(), 0, 0) }
            Rotate { angle: modelRotation.x; axis: Qt.vector3d(1,0,0) }
            Rotate { angle: modelRotation.y; axis: Qt.vector3d(0,1,0) }
            Rotate { angle: modelRotation.z; axis: Qt.vector3d(0,0,1) }
        }
        SceneLoader { id: loaderLeft }
        Entity { components : [loaderLeft, leftTransform] }

        //Right orthotic
        Transform { id: rightTransform
            Translate { translation: rootEntity.isOneModel ?
                                     Qt.vector3d(-0.5 * qmlCppWrapper.rightFoot.scanHeight(),
                                                 -0.5 * qmlCppWrapper.rightFoot.scanWidth(), 0) :
                                     Qt.vector3d(-0.5 * qmlCppWrapper.rightFoot.scanHeight(),
                                                 -qmlCppWrapper.rightFoot.scanWidth(), 0) }
            Rotate { angle: modelRotation.x; axis: Qt.vector3d(1,0,0) }
            Rotate { angle: modelRotation.y; axis: Qt.vector3d(0,1,0) }
            Rotate { angle: modelRotation.z; axis: Qt.vector3d(0,0,1) }
        }
        SceneLoader { id: loaderRight }
        Entity {  components : [loaderRight, rightTransform] }
    }
}
