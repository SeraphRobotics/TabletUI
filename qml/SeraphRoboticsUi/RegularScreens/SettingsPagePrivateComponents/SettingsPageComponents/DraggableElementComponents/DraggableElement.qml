import QtQuick 2.4
import QtGraphicalEffects 1.0

import "../../../../Components"
import "../DraggableElementComponents" 1.0
import "../../"

// Main logic for pad.
// Pad is inside frame which is imageBorder
// in this case.
Rectangle {
    id: imageBorder

    border.color: "#4c4c4c"
    border.width:  frameElementsVisible === true ? 2 : 0

    color : "transparent"

    property int minValue: 20
    property int handleSize: 20

    property bool newObject: true

    property string direction: "" /// left or right.
    property string m_Name: "" // pad name.
    property int m_Type: 0 // pad type.
    property int m_Height: 9  // Height value for specific pad.
    property int m_Depth: 9 // Depth value for specific pad.
    property int m_Stiffness: 50 // Stiffness value for specific pad.
    property var m_OuterLoop: [Qt.point(0, 0),Qt.point(0, 10),Qt.point(10, 10),Qt.point(10, 0)]
    property var m_InnerLoops: [
        [Qt.point(3, 3),Qt.point(3, 7),Qt.point(7, 7),Qt.point(7, 3)]
    ]

    property alias source: exampleImage.source
    property alias elementScale: scale
    property alias _privateElement: _privateElement
    property alias  exampleImage  : exampleImage // Pad image.

    property alias typeNameText: typeNameText // Text which show properites and is connected with pad with line.
    property alias boundingRect: boundingRect // pad image bounding box.

    // Keep basic coordinate and size without scalling
    property int basicY : y/elementScale.yScale
    property int basicX : x/elementScale.xScale
    property int basicWidth :  exampleImage.width/elementScale.xScale
    property int basicHeight :  exampleImage.height/elementScale.yScale

    // Hide or show resize/rotate frame.
    property bool frameElementsVisible: (SettingsPageComponentsSettings.
                                         currentDraggablePad
                                         === imageBorder &&
                                         settingsPageManager.shellModificationsState
                                         !== "padCreator") ? true : false

    // Pad image starting coordinates.
    property int realX : imageBorder.x+exampleImage.x
    property int realY : imageBorder.y+exampleImage.y

    // Value used to bind descriptionConnectionElement to typeNameText horizontal anchor.
    property string labelDirection : direction

    // Current pad should be always on top.
    z : SettingsPageComponentsSettings.currentDraggablePad === imageBorder ?
            99 : 0

    // Recalculate frame size.
    function boxSize()
    {
        var radiany = exampleImage.rotation * (Math.PI / 180)
        var radians = ((radiany> Math.PI * 0.5 && radiany < Math.PI * 1)
                       || (radiany > Math.PI * 1.5 && radiany < Math.PI * 2))?
                    Math.PI - radiany : radiany;
        var widthRect = Math.sin(radians) * exampleImage.height
                + Math.cos(radians) * exampleImage.width
        var heightRect = Math.sin(radians) * exampleImage.width
                + Math.cos(radians) * exampleImage.height

        imageBorder.width = Math.abs(widthRect) + handleSize*elementScale.xScale
        imageBorder.height = Math.abs(heightRect) + handleSize*elementScale.yScale
    }

    // Function used to center image after resizing the frame.
    function boundingBox()
    {
        var radiany = exampleImage.rotation * (Math.PI / 180)
        var radians = ((radiany> Math.PI * 0.5 && radiany < Math.PI * 1)
                       || (radiany > Math.PI * 1.5 && radiany < Math.PI * 2))?
                    Math.PI - radiany : radiany;

        var boundingBox =
                {
            width: 0,
            height: 0,
            widthDiff: 0,
            heightDiff: 0
        };

        boundingBox.width = Math.sin(radians) * exampleImage.height
                + Math.cos(radians) * exampleImage.width
        boundingBox.height = Math.sin(radians) * exampleImage.width
                + Math.cos(radians) * exampleImage.height

        boundingBox.widthDiff =  imageBorder.width - (Math.abs(boundingBox.width)
                                                      + handleSize)
        boundingBox.heightDiff =  imageBorder.height - (Math.abs(boundingBox.height)
                                                        + handleSize)

        return boundingBox;
    }

    // Function used to center frame after rotating the image.
    function boxSizeRotate()
    {
        var radiany = exampleImage.rotation * (Math.PI / 180)
        var radians = ((radiany> Math.PI * 0.5 && radiany < Math.PI * 1)
                       || (radiany > Math.PI * 1.5 && radiany < Math.PI * 2))?
                    Math.PI - radiany : radiany;
        var widthRect = Math.sin(radians) * exampleImage.height + Math.cos(radians)
                * exampleImage.width
        var heightRect = Math.sin(radians) * exampleImage.width + Math.cos(radians)
                * exampleImage.height

        var widthDiff =   imageBorder.width - (Math.abs(widthRect) + handleSize)
        var heigthDiff =  imageBorder.height - (Math.abs(heightRect) + handleSize)

        imageBorder.width =  Math.abs(widthRect) + (handleSize)
        imageBorder.height =  Math.abs(heightRect) + (handleSize)

        imageBorder.x += (widthDiff / 2)
        imageBorder.y += (heigthDiff / 2)
    }

    // Function used to load pad settings into ElementSettings component.
    function setItemAsCurrentItem(type)
    {
        if(SettingsPageComponentsSettings.currentDraggablePad === imageBorder)
        {
            return
        }

        SettingsPageComponentsSettings.resetSliderValue()
        SettingsPageComponentsSettings.currentDraggablePad = imageBorder

        var rootItem = SettingsPageComponentsSettings.m_ShellModificationsComponentPtr
        .elementSettings
        rootItem.imageElementPath = source
        rootItem.elementName.text = m_Name
        rootItem.iconImage.rotation = exampleImage.rotation

        settingsPageManager.shellModificationsState = "elementSettings"
        // set points base values (there is a need to restore them sometimes)
        setCurrentPadBasicValues(SettingsPageComponentsSettings)

        if(direction === "left")
            SettingsPageComponentsSettings.m_CurrentSelectedArea =
                    SettingsPageComponentsSettings.m_LeftSideComponent
        else if(direction === "right")
            SettingsPageComponentsSettings.m_CurrentSelectedArea =
                    SettingsPageComponentsSettings.m_RightSideComponent


        // There is no need to recalculate the frame
        // and the bounding box when settingsPageManager.shellModificationsState.state === "startMovePad"
        if(typeof (type) === "undefined")
        {
            boxSizeRotate()
            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    function restoreValuesToBasicState()
    {
        var basicDraggableElementValues = SettingsPageComponentsSettings.basicDraggableElementValues

        exampleImage.rotation =  basicDraggableElementValues.imageRotation
        exampleImage.width = basicDraggableElementValues.imageWidth*scale.xScale
        exampleImage.height = basicDraggableElementValues.imageHeight*scale.yScale

    }

    function setCurrentPadBasicValues(currentPad)
    {
        var tempCurrentPad = currentPad.basicDraggableElementValues
        tempCurrentPad.imageRotation = exampleImage.rotation
        tempCurrentPad.imageHeight =  exampleImage.height/scale.yScale
        tempCurrentPad.imageWidth =  exampleImage.width/scale.xScale
    }


    function recalculate()
    {
        _privateElement._recalculate()
    }

    Item {
        id : centerElement

        anchors.centerIn: parent
    }

    function calculateYScale()
    {
        var view3d, canvasSize
        if (imageBorder.direction == "left" ) {
            view3d = SettingsPageComponentsSettings.m_LeftSideComponent.view3dRect;
            canvasSize = SettingsPageComponentsSettings.leftCanvasSize;
        } else {
            view3d = SettingsPageComponentsSettings.m_RightSideComponent.view3dRect;
            canvasSize = SettingsPageComponentsSettings.rightCanvasSize;
        }

        if (view3d === null)
            return 1;

        if (view3d.height === 0)
            return 1;

        if (canvasSize.height === -1)
            return 1;

        return view3d.height / canvasSize.height
    }

    function calculateXScale()
    {
        var view3d, canvasSize
        if (imageBorder.direction == "left" ) {
            view3d = SettingsPageComponentsSettings.m_LeftSideComponent.view3dRect;
            canvasSize = SettingsPageComponentsSettings.leftCanvasSize;
        } else {
            view3d = SettingsPageComponentsSettings.m_RightSideComponent.view3dRect;
            canvasSize = SettingsPageComponentsSettings.rightCanvasSize;
        }

        if (view3d === null)
            return 1;

        if (view3d.width === 0)
            return 1;

        if (canvasSize.width === -1)
            return 1;

        return view3d.width / canvasSize.width
    }

    // Scale component for recalculating basic values. If foot gets resized, the component sets proper size and position.
    Scale {
        id: scale

        yScale: calculateYScale()

        onYScaleChanged:
        {
            console.log("Y Scale changed "+yScale)
            imageBorder.y = basicY*yScale
            exampleImage.height = basicHeight*yScale
            boxSize()
        }

        xScale: calculateXScale()

        onXScaleChanged:
        {
            console.log("X scale changed "+xScale+" basic "+basicWidth)
            imageBorder.x = basicX*xScale
            exampleImage.width = basicWidth*xScale
            boxSize()
        }
    }

    /// Bounding box functions.
    DraggableElementPrivate
    {
        id : _privateElement

        currentElement: imageBorder
    }

    // Invisible bounding box used to detect if item is inside or outside foot element.
    Item {
        id: boundingRect
        anchors.centerIn: exampleImage

        parent : imageBorder.parent

        width : recalculate()
        height: recalculate()
    }


    // Pad element
    Image {
        id: exampleImage
        anchors.centerIn: parent

        // Item used for checking image position after rotation.
        Item {
            id: originPoint

            width: 25
            height: 25
            x: 0
            y: 0
        }

        // Recalculates size and position when the pad is created.
        onStatusChanged:
        {
            if(status === Image.Ready && newObject === true)
            {
                if(width == 0)
                    width = sourceSize.width*elementScale.xScale
                if(height == 0)
                    height = sourceSize.height*elementScale.yScale

                imageBorder.width = width+minValue
                imageBorder.height = height+minValue
                newObject = false
                minValue = Math.min(sourceSize.height, sourceSize.width)
                if(minValue > 20)
                    minValue = 20
            }
        }

        // Need to reset rotation values after exceeding 360 degrees.
        onRotationChanged:
        {
            recalculate()
            if(rotation >= 360)
            {
                rotation -= 360
            }
        }
    }

    // Color effects for pad selection.
    ColorOverlay {
        id : coloredImage

        anchors.fill: exampleImage
        source: exampleImage

        rotation: exampleImage.rotation

        color:
        {
            if(settingsPageManager == null)
                return "transparent"

            if(settingsPageManager.shellModificationsState === "padCreator"
                    || (
                        settingsPageManager.shellModificationsState == "startMovePad"
                        && SettingsPageComponentsSettings.currentDraggablePad !== imageBorder
                        )
                    )
                return "#DDDEDC"
            else
                "#4359ce"
        }

        visible :
        {

            if(SettingsPageComponentsSettings.currentDraggablePad === imageBorder
                    || (settingsPageManager != null &&
                        (settingsPageManager.shellModificationsState === "padCreator"
                         ||settingsPageManager.shellModificationsState == "startMovePad")))
                return true
            return false
        }
    }

    MouseArea {
        id : dragArea

        objectName : "draggableElement"

        anchors.fill: parent

        drag.target: parent
        cursorShape: Qt.OpenHandCursor

        visible: (settingsPageManager.modificationState === "3dReview"
                  || (settingsPageManager.shellModificationsState === "padCreator"
                      && direction !== settingsPageManager.currentSelectedDirection) || (
                      ( (settingsPageManager.shellModificationsState === "padCreator"
                         ||  (settingsPageManager.shellModificationsState === "startMovePad"
                              && SettingsPageComponentsSettings.currentDraggablePad != imageBorder))
                       && direction === settingsPageManager.currentSelectedDirection)) ) ? false : true

        onPressed:
        {
            if(direction !== settingsPageManager.currentSelectedDirection
                    && settingsPageManager.currentSelectedDirection !== "both")
            {
                mouse.accepted = false
                return
            }

            ////@note catch starting position into position history.
            if(settingsPageManager.shellModificationsState === "startMovePad"
                    && SettingsPageComponentsSettings.movePadHistory.movePadHistoryCount === -1)
            {
                SettingsPageComponentsSettings.movePadHistory.pushIntoSizeHistory(imageBorder
                                                                                  .basicWidth,
                                                                                  imageBorder.
                                                                                  basicHeight,
                                                                                  imageBorder.basicX,
                                                                                  imageBorder.basicY,
                                                                                  imageBorder.exampleImage.x,
                                                                                  imageBorder.exampleImage.y,
                                                                                  imageBorder.width,
                                                                                  imageBorder.height,
                                                                                  exampleImage.rotation)
            }
            setItemAsCurrentItem()
        }

        onReleased:
        {
            // push into pad size history.
            if(settingsPageManager.shellModificationsState === "startMovePad")
            {
                SettingsPageComponentsSettings.movePadHistory.pushIntoSizeHistory(imageBorder
                                                                                  .basicWidth,
                                                                                  imageBorder.
                                                                                  basicHeight,
                                                                                  imageBorder.basicX,
                                                                                  imageBorder.basicY,
                                                                                  imageBorder.exampleImage.x,
                                                                                  imageBorder.exampleImage.y,
                                                                                  imageBorder.width,
                                                                                  imageBorder.height,
                                                                                  exampleImage.rotation)
            }
        }
    }

    RotatedMouseArea {
        id : rotateElement

        m_Controler : exampleImage
        imageBorder : imageBorder

        visible : frameElementsVisible

        anchors {
            horizontalCenter: imageBorder.horizontalCenter
            bottom : imageBorder.top
            bottomMargin: 50
        }

        mouse_area.onReleased: {
            SettingsPageComponentsSettings.movePadHistory.pushIntoSizeHistory(imageBorder
                                                                              .basicWidth,
                                                                              imageBorder.
                                                                              basicHeight,
                                                                              imageBorder.basicX,
                                                                              imageBorder.basicY,
                                                                              imageBorder.exampleImage.x,
                                                                              imageBorder.exampleImage.y,
                                                                              imageBorder.width,
                                                                              imageBorder.height,
                                                                              exampleImage.rotation)
        }
    }

    GrayDescriptionText {
        id : typeNameText

        visible : settingsPageManager.modificationState === "3dReview" ? true : false

        parent : centerElement

        y : labelDirection === "left" ? -100-imageBorder.height/2+imageBorder.y/3 :
                                        -100-imageBorder.height/2+imageBorder.y/3
        x : labelDirection === "left" ? -220-imageBorder.width/3 : 260+imageBorder.width/3

        z : descriptionConnectionElement.z+1

        text :  qsTr("<p align=\"left\">")+m_Name+qsTr("<br/>")+qsTr("height/depth<br/>△ ")+(m_Height+m_Depth)+qsTr("mm<br/>")
                +qsTr("stiffness<br/>")+m_Stiffness+qsTr("PSI</p>")

        font.pixelSize: 15
    }

    DraggableTextConectionLine {
        id: descriptionConnectionElement

        parent : centerElement
        currentElement: imageBorder
        typeNameText: typeNameText
        m_scale : scale
        opacity : 0.2
        z : 99
    }

    ResizableMouseArea
    {
        id: leftTop

        anchors.top: parent.top
        anchors.left: parent.left

        anchors.topMargin: -handleSize/2
        anchors.leftMargin: -handleSize/2

        draggablePad : imageBorder
        mouseArea.cursorShape: Qt.SizeFDiagCursor
        mouseArea.onPositionChanged:
        {
            if (!mouseArea.pressed)
                return

            var obj = mapToItem(imageBorder.parent, mouse.x, mouse.y)
            var height = 0
            var width = 0


            if (exampleImage.rotation>= 315
                    || exampleImage.rotation >= 0 && exampleImage.rotation < 45
                    || exampleImage.rotation >= 135 && exampleImage.rotation < 235)
            {

                height = exampleImage.height + (lastY - obj.y)
                if(height > minValue)
                {
                    exampleImage.height = exampleImage.height + (lastY - obj.y)
                    lastY = obj.y
                }

                width = exampleImage.width + (lastX - obj.x)
                if(width > minValue)
                {
                    exampleImage.width = exampleImage.width + (lastX - obj.x)
                    lastX = obj.x
                }

            } else if (exampleImage.rotation >= 45 && exampleImage.rotation < 135
                       || exampleImage.rotation >= 235 && exampleImage.rotation < 315)
            {

                height = exampleImage.width + (lastY - obj.y)
                if(height > minValue)
                {
                    exampleImage.width = exampleImage.width + (lastY - obj.y)
                    lastY = obj.y
                }

                width = exampleImage.height + (lastX - obj.x)
                if(width > minValue)
                {
                    exampleImage.height = exampleImage.height + (lastX - obj.x)
                    lastX = obj.x
                }
            }

            var bBox = boundingBox();

            imageBorder.y = imageBorder.y + bBox.heightDiff
            imageBorder.x = imageBorder.x + bBox.widthDiff
            imageBorder.width += -bBox.widthDiff
            imageBorder.height += -bBox.heightDiff

            delete bBox;
            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    ResizableMouseArea
    {
        id: rightTop

        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: -handleSize/2
        anchors.rightMargin: -handleSize/2

        rotation : 90
        draggablePad : imageBorder
        mouseArea.cursorShape: Qt.SizeBDiagCursor
        mouseArea.onPositionChanged:
        {
            if (!mouseArea.pressed)
                return

            var obj = mapToItem(imageBorder.parent, mouse.x, mouse.y)
            var height = 0
            var width = 0

            if (exampleImage.rotation>= 315
                    || exampleImage.rotation >= 0 && exampleImage.rotation < 45
                    || exampleImage.rotation >= 135 && exampleImage.rotation < 235) {

                height = exampleImage.height + (lastY - obj.y)
                if(height > minValue) {
                    exampleImage.height = exampleImage.height + (lastY - obj.y)
                    lastY = obj.y
                }

                width = exampleImage.width + (obj.x -lastX)
                if(width > minValue) {
                    exampleImage.width = exampleImage.width + (obj.x -lastX)
                    lastX = obj.x
                }

            } else if (exampleImage.rotation >= 45 && exampleImage.rotation < 135
                       || exampleImage.rotation >= 235 && exampleImage.rotation < 315){

                height = exampleImage.width + (lastY - obj.y)
                if(height > minValue) {
                    exampleImage.width = exampleImage.width + (lastY - obj.y)
                    lastY = obj.y
                }

                width = exampleImage.height + (obj.x - lastX)
                if(width > minValue){
                    exampleImage.height = exampleImage.height + (obj.x - lastX)
                    lastX = obj.x
                }
            }

            var bBox = boundingBox();

            imageBorder.y = imageBorder.y + bBox.heightDiff
            imageBorder.width += -bBox.widthDiff
            imageBorder.height += -bBox.heightDiff

            delete bBox;
            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    ResizableMouseArea
    {
        id: leftDown

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: -handleSize/2
        anchors.leftMargin: -handleSize/2

        rotation : 270
        draggablePad : imageBorder
        mouseArea.cursorShape: Qt.SizeBDiagCursor
        mouseArea.onPositionChanged:
        {
            if (!mouseArea.pressed)
                return

            var obj = mapToItem(imageBorder.parent, mouse.x, mouse.y)
            var height = 0
            var width = 0

            if (exampleImage.rotation>= 315
                    || exampleImage.rotation >= 0 && exampleImage.rotation < 45
                    || exampleImage.rotation >= 135 && exampleImage.rotation < 235) {

                height = exampleImage.height + (obj.y - lastY)
                if(height > minValue) {
                    exampleImage.height = exampleImage.height + (obj.y - lastY)
                    lastY = obj.y
                }

                width = exampleImage.width + (lastX - obj.x)
                if(width > minValue) {
                    exampleImage.width = exampleImage.width + (lastX - obj.x)
                    lastX = obj.x
                }

            } else if (exampleImage.rotation >= 45 && exampleImage.rotation < 135
                       || exampleImage.rotation >= 235 && exampleImage.rotation < 315){

                height = exampleImage.width + (obj.y - lastY)
                if(height > minValue) {
                    exampleImage.width = exampleImage.width + (obj.y - lastY)
                    lastY = obj.y
                }

                width = exampleImage.height + (lastX - obj.x)
                if(width > minValue){
                    exampleImage.height = exampleImage.height + (lastX - obj.x)
                    lastX = obj.x
                }
            }

            var bBox = boundingBox();

            imageBorder.x = imageBorder.x + bBox.widthDiff
            imageBorder.width += -bBox.widthDiff
            imageBorder.height += -bBox.heightDiff

            delete bBox;
            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    ResizableMouseArea
    {
        id: rightDown

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: -handleSize/2
        anchors.rightMargin: -handleSize/2

        rotation : 180
        draggablePad : imageBorder
        mouseArea.cursorShape: Qt.SizeFDiagCursor
        mouseArea.onPositionChanged:
        {
            if (!mouseArea.pressed)
                return

            var obj = mapToItem(imageBorder.parent, mouse.x, mouse.y)
            var height = 0
            var width = 0

            if (exampleImage.rotation>= 315
                    || exampleImage.rotation >= 0 && exampleImage.rotation < 45
                    || exampleImage.rotation >= 135 && exampleImage.rotation < 235) {

                height = exampleImage.height + (obj.y - lastY)
                if(height > minValue) {
                    exampleImage.height = exampleImage.height + (obj.y - lastY)
                    lastY = obj.y
                }

                width = exampleImage.width + (obj.x -lastX)
                if(width > minValue) {
                    exampleImage.width = exampleImage.width + (obj.x -lastX)
                    lastX = obj.x
                }

            } else if (exampleImage.rotation >= 45 && exampleImage.rotation < 135
                       || exampleImage.rotation >= 235 && exampleImage.rotation < 315){

                height = exampleImage.width + (obj.y - lastY)
                if(height > minValue) {
                    exampleImage.width = exampleImage.width + (obj.y - lastY)
                    lastY = obj.y
                }

                width = exampleImage.height + (obj.x -lastX)
                if(width > minValue){
                    exampleImage.height = exampleImage.height + (obj.x -lastX)
                    lastX = obj.x
                }
            }

            var bBox = boundingBox();

            imageBorder.width += -bBox.widthDiff
            imageBorder.height += -bBox.heightDiff

            delete bBox;

            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    onXChanged:
    {
        console.log("X changed "+x)
        if(dragArea.pressed === true)
        {
            boxSizeRotate()
            recalculate()
            _privateElement._checkDropAreas()
        }
    }
    onYChanged:
    {
        console.log("Y changed "+y)
        if(dragArea.pressed === true)
        {
            boxSizeRotate()
            recalculate()
            _privateElement._checkDropAreas()
        }
    }

    Component.onDestruction:
    {
        SettingsPageComponentsSettings.deleteElementFromArrayOnElementDestroy(imageBorder)
    }

}
