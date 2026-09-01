pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Effects
import "."

Item {
    id: root

    required property var model
    property bool cycle: false
    property real itemSpacing: 0
    property real itemContainerRadius: 28
    property color itemColor: "transparent"
    property color itemHoverColor: "#484459"
    property color itemContainerColor: "#312e41"
    property int itemHeight: 50
    property int itemHeightDescription: 65
    property color textColor: "#e5e1e9"
    property color textColorSecondary: "#c9c5d0"
    property int fontSize: 16
    property int fontSizeSecondary: 12
    property bool fontBold: false
    property int hoveredIndex: -1
    property alias currentIndex: list.currentIndex

    signal activated(string command)

    function moveSelection(delta) {
        if (list.count <= 0)
            return

        var next = list.currentIndex + delta
        if (root.cycle) {
            if (next < 0)
                next = list.count - 1
            else if (next >= list.count)
                next = 0
        } else {
            next = Math.max(0, Math.min(list.count - 1, next))
        }

        list.currentIndex = next
        list.positionViewAtIndex(next, ListView.Contain)
    }

    Rectangle {
        anchors.fill: parent
        radius: root.itemContainerRadius
        color: root.itemContainerColor
    }

    Rectangle {
        id: itemContainerMask
        anchors.fill: parent
        radius: root.itemContainerRadius
        color: "white"
        visible: false
        layer.enabled: true
    }

    ListView {
        id: list

        anchors.fill: parent
        model: root.model
        currentIndex: 0
        spacing: root.itemSpacing
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        highlightRangeMode: ListView.ApplyRange
        preferredHighlightBegin: 0
        preferredHighlightEnd: height
        
        layer.enabled: true

        layer.effect: MultiEffect {
            maskEnabled: true
            maskSource: itemContainerMask
            maskThresholdMin: 0.0
            maskThresholdMax: 1.0
        }

        delegate: MenuItem {
            normalColor: root.itemColor
            hoverColor: root.itemHoverColor
            currentColor: root.itemHoverColor
            textColor: root.textColor
            secondaryTextColor: root.textColorSecondary
            fontSize: root.fontSize
            secondaryFontSize: root.fontSizeSecondary
            fontBold: root.fontBold
            normalHeight: root.itemHeight
            descriptionHeight: root.itemHeightDescription
            isHovered: root.hoveredIndex === index
            isCurrent: ListView.isCurrentItem

            onHovered: function(itemIndex) {
                root.hoveredIndex = itemIndex
            }

            onExited: function(itemIndex) {
                if (root.hoveredIndex === itemIndex)
                    root.hoveredIndex = -1
            }

            onClicked: function(command) {
                root.activated(command)
            }
        }
    }
}
