import QtQuick 2.1
import QtQuick.Window 2.1

Window {
    width: 200
    height: 200
    Rectangle {
        color: "green"
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            onClicked: Qt.quit()
        }
        Text {
            anchors.centerIn: parent
            text: "quit child "+pid
        }
    }
    Component.onCompleted: show()
}

