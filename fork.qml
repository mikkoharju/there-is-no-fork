import QtQml 2.0

QtObject {
    property var other: LoadedByFork {}
    property var importsQtQuick: ImportsQtQuick {}
    Component.onCompleted: console.log("Hello from file!")
}
