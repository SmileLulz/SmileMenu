pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "."

Item {
    id: root

    required property var model
    property bool cycle: false
    property int hoveredIndex: -1
    property alias currentIndex: grid.currentIndex

    signal activated(string command)

    readonly property int columns: 4
    readonly property int cellHeight: Theme.sizes.cellHeight

    property color containerColor: Theme.colors.surface
    property color maskColor: "white"
    property real maskRadius: Theme.sizes.maskRadius

    function moveHorizontal(delta) {
        if (grid.count <= 0) return
        var next = grid.currentIndex + delta
        if (cycle) next = (next + grid.count) % grid.count
        else next = Math.max(0, Math.min(grid.count - 1, next))
        grid.currentIndex = next
        grid.positionViewAtIndex(next, GridView.Contain)
    }

    function moveVertical(delta) {
        if (grid.count <= 0) return
        var current = grid.currentIndex
        var next = current + delta * columns
        if (cycle) {
            if (delta < 0 && current < columns) next = grid.count - 1
            else if (delta > 0 && current >= grid.count - columns) next = 0
            else next = Math.max(0, Math.min(grid.count - 1, next))
        } else {
            next = Math.max(0, Math.min(grid.count - 1, next))
        }
        grid.currentIndex = next
        grid.positionViewAtIndex(next, GridView.Contain)
    }

    Rectangle {
        anchors.fill: parent
        radius: root.maskRadius
        color: root.containerColor
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        radius: root.maskRadius
        color: root.maskColor
        visible: false
        layer.enabled: true
    }

    GridView {
        id: grid

        anchors.fill: parent
        model: root.model
        cellWidth: width / root.columns
        cellHeight: root.cellHeight
        currentIndex: count > 0 ? 0 : -1
        clip: true
        interactive: true
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.horizontal: ScrollBar { visible: false }
        ScrollBar.vertical: ScrollBar { visible: false }

        layer.enabled: true
        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: mask
        }

        delegate: Item {
            id: delegateRoot

            required property var modelData
            required property int index

            readonly property bool current: GridView.isCurrentItem

            width: grid.cellWidth
            height: root.cellHeight

            MenuItem {
                anchors.fill: parent

                itemName: modelData ? modelData.name : ""
                itemDescription: modelData ? (modelData.description || "") : ""
                itemIcon: modelData ? (modelData.icon || "") : ""
                itemCommand: modelData ? modelData.command : ""
                itemIndex: index

                normalColor: "transparent"
                hoverColor: Theme.colors.hover
                currentColor: Theme.colors.current
                textColor: Theme.colors.text
                secondaryTextColor: Theme.colors.textSecondary
                itemNameFontSize: Theme.sizes.itemNameSize
                itemDescFontSize: Theme.sizes.itemDescSize
                iconSize: Theme.sizes.iconSize

                isCurrent: delegateRoot.current
                isHovered: root.hoveredIndex === index

                onHovered: root.hoveredIndex = index
                onExited: {
                    if (root.hoveredIndex === index)
                        root.hoveredIndex = -1
                }
                onClicked: root.activated(itemCommand)
            }
        }
    }
}
