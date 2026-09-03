import QtQuick

Rectangle {
    id: root

    required property string itemName
    required property string itemDescription
    required property string itemIcon
    required property string itemCommand
    required property int itemIndex

    property bool isHovered: false
    property bool isCurrent: false

    property color normalColor: "transparent"
    property color hoverColor: "#484459"
    property color currentColor: "#5b5670"
    property color textColor: "#e5e1e9"
    property color secondaryTextColor: "#c9c5d0"
    property int itemNameFontSize: 14
    property int itemDescFontSize: 11
    property int iconSize: 86

    signal clicked(string command)
    signal hovered(int itemIndex)
    signal exited(int itemIndex)

    radius: 2

    color: {
        if (isCurrent)
            return currentColor
        if (isHovered)
            return hoverColor
        return normalColor
    }

    Column {
        anchors.centerIn: parent
        width: parent.width - 16
        // spacing: 4

        Image {
            width: root.iconSize
            height: root.iconSize
            anchors.horizontalCenter: parent.horizontalCenter
            source: root.itemIcon ? "image://icons/" + root.itemIcon : ""
            fillMode: Image.PreserveAspectFit
            visible: root.itemIcon !== ""
            asynchronous: true
        }

        // Text {
        //     width: parent.width
        //     text: root.itemName
        //     color: root.textColor
        //     font.pixelSize: root.itemNameFontSize
        //     horizontalAlignment: Text.AlignHCenter
        //     elide: Text.ElideRight
        //     maximumLineCount: 1
        // }

        // Text {
        //     width: parent.width
        //     text: root.itemDescription
        //     visible: root.itemDescription !== ""
        //     color: root.secondaryTextColor
        //     font.pixelSize: root.itemDescFontSize
        //     horizontalAlignment: Text.AlignHCenter
        //     elide: Text.ElideRight
        //     maximumLineCount: 1
        // }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: root.hovered(root.itemIndex)
        onExited: {
            if (root.isHovered)
                root.exited(root.itemIndex)
        }
        onClicked: root.clicked(root.itemCommand)
    }
}
