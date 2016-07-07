// EdgeEditor.qml

import QtQuick 2.2

import "drawing.js" as Drawing

Item {
    id: root

    //// @note in future we need something automatic to edge detection like openCv.

    property string leftImageSource: ""
    property string rightImageSource: ""
    property alias canvas : canvas
    property int initWidth
    property int initHeight
    property int edgeRadius: 25
    property real scaleFactor: 1.0

    // Contains draggable points for foot: fore and heal (always without scaling to canvas)
    property var forePoints: []
    property var healPoints: []

    // calls only when new scan images were generated
    function initFeet()
    {
        img.source = leftImageSource
        var leftViewScale = canvas.width / qmlCppWrapper.leftFoot.scanWidth()
        img.source = rightImageSource
        var rightViewScale = canvas.width / qmlCppWrapper.rightFoot.scanWidth()
        qmlCppWrapper.initFeet(leftViewScale, rightViewScale)
    }

    // update EdgeEditor settings when current foot was changed
    function useCurrentFoot()
    {
        canvas.updateViewScale()
        qmlCppWrapper.currentFoot.canvasScale = canvas.viewScale;
        forePoints = qmlCppWrapper.currentFoot.controlPoints.forePoints
        healPoints = qmlCppWrapper.currentFoot.controlPoints.healPoints
        canvas.requestPaint()
    }

    // Function calls when user selects the left foot
    function setLeftEdge()
    {
        if (leftImageSource === "")
            return

        qmlCppWrapper.currentFoot = qmlCppWrapper.leftFoot
        img.source = leftImageSource
        useCurrentFoot()
    }

    // Function calls when user selects the right foot
    function setRightEdge()
    {
        if (rightImageSource === "")
            return

        qmlCppWrapper.currentFoot = qmlCppWrapper.rightFoot
        img.source = rightImageSource
        useCurrentFoot()
    }

    function checkEdgeClicked(edge, x, y) {
        var pointX = edge.x
        var pointY = edge.y
        var xs = (x - pointX)
        var ys = (y - pointY)
        return (Math.sqrt(xs * xs + ys * ys) < edgeRadius)
    }

    // Function detects which draggable points user drags
    function detectClickedEdge(x, y) {
        for (var i in forePoints) {
            if (checkEdgeClicked(forePoints[i], x, y)) {
                return forePoints[i];
            }
        }

        for (var j in healPoints) {
            if (checkEdgeClicked(healPoints[j], x, y)) {
                return healPoints[j];
            }
        }

        return null;
    }

    clip: true

    Image {
        id: img
        visible: true
        sourceSize.width: parent.width
        sourceSize.height: parent.height
        cache: false
    }

    MouseArea {
        id: mouseArea
        property var currentEdge: null
        anchors.fill: canvas

        onPressed: {
            currentEdge = parent.detectClickedEdge(mouseX, mouseY)
        }

        onPositionChanged: {
            if (!mouseArea.pressed || currentEdge === null)
                return

            if (mouseX < edgeRadius || mouseX + edgeRadius > canvas.width)
                return

            if (mouseY < edgeRadius || mouseY + edgeRadius > canvas.height)
                return

            currentEdge.x = mouseX
            currentEdge.y = mouseY

            canvas.requestPaint()
        }

        onReleased: {
            if (currentEdge !== null) {
                // recalculate boundary points only after user released any draggable point
                qmlCppWrapper.recalculateBoundary(forePoints.concat(healPoints), canvas.viewScale);
            }
        }
    }

    Canvas {
        id: canvas

        width: img.width
        height: img.height
        contextType: "2d"

        // update view scale after setting new image source
        function updateViewScale()
        {
            viewScale = width / qmlCppWrapper.currentFoot.scanWidth()
        }

        property real viewScale: 0.0

        onPaint: {
            if (viewScale == 0.0)
                return

            context.save()
            context.clearRect(0, 0, width, height)

            context.strokeStyle = "darkblue"
            context.lineWidth = 5
            context.lineJoin = "round"

            context.beginPath()
            // draw boundary curve
            var boundaryPoints = qmlCppWrapper.currentFoot.getBoundaryPoints(viewScale);
            var points = [];
            for(var i in boundaryPoints) {
                points.push(boundaryPoints[i].x);
                points.push(boundaryPoints[i].y);
            }

            Drawing.drawSpline(context, points, 0.5)
            context.closePath()
            context.fillStyle = "rgba(0, 0, 120, 0.4)"
            context.fill()

            // draw draggable points
            // fore points
            for (var k in forePoints) {
                Drawing.drawEdge(context,
                                 forePoints[k].x,
                                 forePoints[k].y,
                                 edgeRadius,
                                 Qt.rgba(1, 0.5, 0, 1));
            }

            // heal points
            for (var j in healPoints) {
                Drawing.drawEdge(context,
                                 healPoints[j].x,
                                 healPoints[j].y,
                                 edgeRadius,
                                 Qt.rgba(1, 1, 0, 1));
            }
            context.restore()
        }

        onCanvasSizeChanged: {
            if (canvas.available) {
                if (context !== null) {
                    // for scaling
                    scaleFactor = canvas.height / initHeight

                    // According to bug in QtQuick:
                    //   https://bugreports.qt-project.org/browse/QTBUG-39114
                    getContext('2d').reset()
                    requestPaint()
                }
            }
        }

        onWidthChanged: {
            canvas.requestPaint()
        }
        onHeightChanged: {
            canvas.requestPaint()
        }

        onViewScaleChanged: {
            qmlCppWrapper.currentFoot.canvasScale = canvas.viewScale;
        }
    }

    Connections {
        target: qmlCppWrapper

        onBoundaryUpdated: {
            // request new points only if boundary was updated
            forePoints = qmlCppWrapper.currentFoot.controlPoints.forePoints
            healPoints = qmlCppWrapper.currentFoot.controlPoints.healPoints
            canvas.requestPaint()
        }
    }

    states : [
        State {
            name : "left"
            StateChangeScript {
                script: setLeftEdge();
            }
        },
        State {
            name : "right"
            StateChangeScript {
                script: setRightEdge();
            }
        }

    ]
}
