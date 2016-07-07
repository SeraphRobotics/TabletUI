import QtQuick 2.0

Item {
    anchors.fill: parent

    signal show();
    signal hide();

    visible: false

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.5
    }

    MouseArea {
        anchors.fill: parent
    }

    onShow: {
        visible = true;
    }

    onHide: {
        visible = false;
    }
}

