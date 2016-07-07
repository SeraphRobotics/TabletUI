import QtQuick 2.5

GenericOverlay {
    anchors.fill: parent

    property alias text: text.text

    Image {
        id: rotatingBox
        anchors.centerIn: parent
        width: 50
        height: 50
        source: "qrc:/QmlResources/loader.svg"

        RotationAnimator {
            target: rotatingBox;
            from: 0;
            to: 360;
            loops:  RotationAnimator.Infinite
            duration: 1500
            running: true
        }
    }
    Text {
        id: text
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: rotatingBox.bottom
        color: "white"
        font {
            pixelSize: 25
        }
    }
}

