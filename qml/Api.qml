pragma Singleton

import QtQml

QtObject {
    readonly property var apps: launcher ? launcher.apps : []
    readonly property string promptText: launcher ? launcher.promptText : ""
    readonly property string promptPositionText: launcher ? launcher.promptPositionText : "entry"
    readonly property string placeholder: launcher ? launcher.placeholder : "Search..."
    readonly property int windowWidth: launcher ? launcher.windowWidth : 500
    readonly property int maxVisibleItems: launcher ? launcher.maxVisibleItems : 6
    readonly property int minVisibleItems: launcher ? launcher.minVisibleItems : 1
    readonly property bool showTextField: launcher ? launcher.showTextField : true
    readonly property bool cycle: launcher ? launcher.cycle : false

    function search(text) {
        if (launcher)
            launcher.search(text)
    }

    function launch(command) {
        if (launcher)
            launcher.launch(command)
    }
}
