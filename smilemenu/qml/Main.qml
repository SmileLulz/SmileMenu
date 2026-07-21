import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import org.kde.layershell as LayerShellQt

Window {
    id: root

    property bool closing: false

    property real window_radius: theme ? theme.window_radius : 40
    property color background_color: theme ? theme.background_color : "#303446"

    property bool show_text_field: launcher ? launcher.show_text_field : true

    property real item_spacing: theme ? theme.item_spacing : 0
    property real item_radius_high: theme ? theme.item_radius_high : 28
    property real item_radius_low: theme ? theme.item_radius_low : 0
    property color item_color: theme ? theme.item_color : "transparent"
    property color item_hover_color: theme ? theme.item_hover_color : "#626880"
    property color item_container_color: theme ? theme.item_container_color : "#414559"
    property int item_height: theme ? theme.item_height : 50
    property int item_height_description: theme ? theme.item_height_description : 65

    property int min_visible_items: launcher ? launcher.min_visible_items : 1
    property int max_visible_items: launcher ? launcher.max_visible_items : 6

    property int content_margins: theme ? theme.content_margins : 28
    property int content_spacing: theme ? theme.content_spacing : 14

    property color text_color: theme ? theme.text_color : "#c6d0f5"
    property color text_color_secondary: theme ? theme.text_color_secondary : "#949cbb"
    property int font_size: theme ? theme.font_size : 16
    property int font_size_secondary: theme ? theme.font_size_secondary : 12
    property bool font_bold: theme ? theme.font_bold : false

    property color text_field_placeholder_color: theme ? theme.text_field_placeholder_color : "#737994"
    property color text_field_color: theme ? theme.text_field_color : "transparent"
    property color text_field_border_color: theme ? theme.text_field_border_color : "transparent"
    property real text_field_border_width: theme ? theme.text_field_border_width : 0
    property real text_field_radius: theme ? theme.text_field_radius : 0

    visible: true
    color: "transparent"

    width: launcher ? launcher.window_width : 500
    height: {
        var count = launcher ? launcher.apps.length : 0

        var visibleItems = Math.max(
            min_visible_items,
            Math.min(count, max_visible_items)
        )

        var height = content_margins * 2
        var hasTopPrompt = root.prompt_exists && root.prompt_text === "top"
        var hasEntryPrompt = root.prompt_exists && root.prompt_text === "entry"

        if (hasTopPrompt) {
            height += 30
            height += content_spacing
        }

        if (root.show_text_field || hasEntryPrompt) {
            height += 50
            height += content_spacing
        }

        height += (
            visibleItems * item_height +
            Math.max(0, visibleItems - 1) * item_spacing
        )

        return height
    }

    LayerShellQt.Window.layer: LayerShellQt.Window.LayerOverlay
    LayerShellQt.Window.keyboardInteractivity: LayerShellQt.Window.KeyboardInteractivityExclusive
    LayerShellQt.Window.anchors: LayerShellQt.Window.AnchorNone
    LayerShellQt.Window.wantsToBeOnActiveScreen: true

    readonly property bool prompt_exists: launcher && launcher.prompt_text !== ""
    readonly property string prompt_text: prompt_exists ? launcher.prompt_position_text : ""


    FocusScope {
        id: keyboardFocus
        anchors.fill: parent
        focus: true

        Keys.onDownPressed: {
            if (list.count > 0) {
                list.currentIndex = Math.min(
                    list.count - 1,
                    list.currentIndex + 1
                )

                list.positionViewAtIndex(
                    list.currentIndex,
                    ListView.Contain
                )
            }
        }

        Keys.onUpPressed: {
            if (list.count > 0) {
                list.currentIndex = Math.max(
                    0,
                    list.currentIndex - 1
                )

                list.positionViewAtIndex(
                    list.currentIndex,
                    ListView.Contain
                )
            }
        }

        Keys.onReturnPressed: {
            if (list.currentItem) {
                launcher.launch(list.currentItem.command)
                root.closeAnimated()
            }
        }

        Keys.onEscapePressed: {
            root.closeAnimated()
        }
    }


    Rectangle {
        id: container
        anchors.fill: parent
        radius: root.window_radius
        color: root.background_color
        opacity: 0
        scale: 0
        transformOrigin: Item.Center

        Behavior on opacity {
            NumberAnimation {
                duration: 140
                easing.type: Easing.OutCubic
            }
        }

        Behavior on scale {
            NumberAnimation {
                duration: 180
                easing.type: Easing.OutCubic
            }
        }

        Column {
            anchors.fill: parent
            anchors.margins: root.content_margins
            spacing: root.content_spacing

            Text {
                visible: root.prompt_exists && root.prompt_text === "top"
                text: launcher ? launcher.prompt_text : ""
                color: root.text_color
                font.pixelSize: 24
            }

            RowLayout {
                spacing: 10
                width: parent.width
                height: visible ? 50 : 0
                visible: root.show_text_field || (root.prompt_exists && root.prompt_text === "entry")

                Loader {
                    id: promptLoader
                    visible: root.prompt_exists && root.prompt_text === "entry"
                    active: root.prompt_exists && root.prompt_text === "entry"

                    sourceComponent: Text {
                        text: launcher ? launcher.prompt_text : ""
                        color: root.text_color
                        font.pixelSize: 18
                    }
                }

                TextField {
                    id: search
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    Keys.forwardTo: [keyboardFocus]
                    visible: root.show_text_field
                    enabled: root.show_text_field
                    focus: true
                    placeholderText: customPlaceholder ? customPlaceholder : "Search..."
                    color: root.text_color
                    placeholderTextColor: root.text_field_placeholder_color
                    font.pixelSize: root.font_size
                    font.bold: root.font_bold

                    background: Rectangle {
                        color: root.text_field_color
                        border.color: root.text_field_border_color
                        border.width: root.text_field_border_width
                        radius: root.text_field_radius
                    }

                    onTextChanged: {
                        if (launcher) {
                            launcher.search(text)
                            list.currentIndex = 0
                            list.hoveredIndex = -1
                        }
                    }
                }
            }

            Item {
                width: parent.width
                height: {
                    var count = launcher ? launcher.apps.length : 0
                    var visibleItems = Math.max(root.min_visible_items, Math.min(count, root.max_visible_items))
                    return (
                        visibleItems * item_height +
                        Math.max(0, visibleItems - 1) * root.item_spacing
                    )
                }

                Rectangle {
                    anchors.fill: parent
                    radius: root.item_radius_high
                    color: root.item_container_color
                }

                ListView {
                    id: list
                    model: launcher ? launcher.apps : []
                    currentIndex: 0
                    spacing: root.item_spacing
                    anchors.fill: parent
                    clip: true

                    boundsBehavior: Flickable.StopAtBounds
                    highlightRangeMode: ListView.ApplyRange
                    preferredHighlightBegin: 0
                    preferredHighlightEnd: height

                    layer.enabled: true
                    layer.effect: OpacityMask {
                        maskSource: Rectangle {
                            width: list.width
                            height: list.height
                            radius: root.item_radius_high
                        }
                    }

                    property int hoveredIndex: -1

                    delegate: Rectangle {
                        id: item
                        width: list.width
                        height: modelData.description ? root.item_height_description : root.item_height

                        property bool isHovered: list.hoveredIndex === index
                        property bool isCurrent: ListView.isCurrentItem

                        color: {
                            if (isHovered && !isCurrent) {
                                return root.item_hover_color
                            }
                            return isCurrent ? root.item_hover_color : root.item_color
                        }

                        topLeftRadius: index === 0 ? root.item_radius_high : root.item_radius_low
                        topRightRadius: index === 0 ? root.item_radius_high : root.item_radius_low
                        bottomLeftRadius: index === list.count - 1 ? root.item_radius_high : root.item_radius_low
                        bottomRightRadius: index === list.count - 1 ? root.item_radius_high : root.item_radius_low

                        property var command: modelData.command

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
                                fillMode: Image.PreserveAspectFit
                                visible: modelData.icon && modelData.icon !== ""
                            }

                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 2

                                Text {
                                    text: modelData.name
                                    font.pixelSize: root.font_size
                                    color: root.text_color
                                }

                                Text {
                                    visible: modelData.description && modelData.description !== ""
                                    text: modelData.description
                                    font.pixelSize: root.font_size_secondary
                                    color: root.text_color_secondary
                                }
                            }
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            hoverEnabled: true

                            onEntered: {
                                list.hoveredIndex = index
                            }

                            onExited: {
                                if (list.hoveredIndex === index) {
                                    list.hoveredIndex = -1
                                }
                            }

                            onClicked: {
                                launcher.launch(item.command)
                                root.closeAnimated()
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        container.opacity = 1
        container.scale = 1

        Qt.callLater(function() {
            keyboardFocus.forceActiveFocus()

            if (root.show_text_field) {
                search.forceActiveFocus()
            }
        })
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            root.closeAnimated()
        }
    }

    Timer {
        id: closeTimer
        interval: 180
        repeat: false
        onTriggered: root.close()
    }

    function closeAnimated() {
        if (closing) return
        closing = true
        container.opacity = 0
        container.scale = 0.8
        closeTimer.start()
    }
}