pragma Singleton
import QtQuick

QtObject {
    readonly property QtObject colors: QtObject {
        readonly property color background: "#1c192b"
        readonly property color surface: "#312e41"
        readonly property color text: "#e5e1e9"
        readonly property color textSecondary: "#c9c5d0"
        readonly property color hover: "#484459"
        readonly property color current: "#5b5670"
        readonly property color placeholder: "#938f99"
        readonly property color fieldBackground: "transparent"
        readonly property color fieldBorder: "transparent"
    }

    readonly property QtObject sizes: QtObject {
        // Common
        readonly property int windowRadius: 40
        readonly property int contentMargins: 28
        readonly property int contentSpacing: 14
        readonly property int fontSize: 16
        readonly property int fontSizeSecondary: 12
        readonly property bool fontBold: false
        readonly property int fieldRadius: 0
        readonly property int fieldBorderWidth: 0

        // Grid preset
        readonly property int cellHeight: 92
        readonly property int iconSize: 86
        readonly property int itemNameSize: 14
        readonly property int itemDescSize: 11
        readonly property real maskRadius: 28

        // List preset
        readonly property int itemHeight: 50
        readonly property int itemHeightDescription: 65
        readonly property real itemSpacing: 0
        readonly property real itemContainerRadius: 28
        readonly property int listIconSize: 32
    }
}
