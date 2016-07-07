import QtQuick 2.0

Item {
    id : defaultValueIcon

    width : 60
    height : 20

    Image {
        id : icon

        source : "qrc:/QmlResources/pointer-default.png"

        anchors {
            verticalCenter: parent.verticalCenter
            left : parent.left
            leftMargin : 0
        }
    }

    GrayDescriptionText {
        text : qsTr("default")

        font.pixelSize: 13

        anchors {
            verticalCenter: parent.verticalCenter
            left : icon.right
            leftMargin: 5
        }
    }

}
