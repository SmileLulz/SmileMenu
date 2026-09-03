import QtQuick

Rectangle {
    id: root

    required property var modelData
    required property int index

    property real normalRadius: 0
    property real topRadius: 0
    property real bottomRadius: 0
    property color normalColor: "transparent"
    property color hoverColor: "transparent"
    property color currentColor: "transparent"
    property color textColor: "#e5e1e9"
    property color secondaryTextColor: "#c9c5d0"
    property int fontSize: 16
    property int secondaryFontSize: 12
    property bool fontBold: false
    property int normalHeight: 50
    property int descriptionHeight: 65

    property bool isHovered: false
    property bool isCurrent: false

    signal clicked(string command)
    signal hovered(int itemIndex)
    signal exited(int itemIndex)

    width: parent ? parent.width : 0
    height: modelData.description ? descriptionHeight : normalHeight

    radius: Math.max(topRadius, Math.max(bottomRadius, normalRadius))

    color: {
        if (isHovered && !isCurrent)
            return hoverColor
        return isCurrent ? currentColor : normalColor
    }

    clip: true

    Row {
        spacing: 12
        anchors {
            fill: parent
            leftMargin: 12
            rightMargin: 12
        }

        Image {
            id: appIcon
            source: modelData.icon ? "image://icons/" + modelData.icon : ""
            width: 32
            height: 32
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectCrop
            visible: modelData.icon && modelData.icon !== ""
            asynchronous: true
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            width: Math.max(0, parent.width - 44)
            spacing: 2

            Text {
                text: modelData.name
                width: parent.width
                elide: Text.ElideRight
                maximumLineCount: 1
                clip: true
                font.pixelSize: root.fontSize
                font.bold: root.fontBold
                color: root.textColor
            }

            Text {
                visible: modelData.description && modelData.description !== ""
                text: modelData.description
                width: parent.width
                elide: Text.ElideRight
                maximumLineCount: 1
                clip: true
                font.pixelSize: root.secondaryFontSize
                color: root.secondaryTextColor
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: root.hovered(root.index)
        onExited: root.exited(root.index)
        onClicked: root.clicked(root.modelData.command)
    }
}
