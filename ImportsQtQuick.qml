import QtQuick 2.1
import QtQuick.Window 2.1

Loader {
    property var other: LoadedByFork {}
    asynchronous: true
    source: "AsyncLoaded.qml"
    property int s: Screen.width
    Component.onCompleted: console.log(s)
}
