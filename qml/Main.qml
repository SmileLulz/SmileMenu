import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import org.kde.layershell as LayerShellQt

Window {
    id: root

    property bool closing: false

    property real window_radius: 40
    property color background_color: "#1c192b"

    property bool show_text_field: launcher ? launcher.showTextField : true

    property real item_spacing: 0
    property real item_radius_high: 28
    property real item_radius_low: 0
    property color item_color: "transparent"
    property color item_hover_color: "#484459"
    property color item_container_color: "#312e41"
    property int item_height: 50
    property int item_height_description: 65

    property int min_visible_items: launcher ? launcher.minVisibleItems : 1
    property int max_visible_items: launcher ? launcher.maxVisibleItems : 6

    property int content_margins: 28
    property int content_spacing: 14

    property color text_color: "#e5e1e9"
    property color text_color_secondary: "#c9c5d0"
    property int font_size: 16
    property int font_size_secondary: 12
    property bool font_bold: false

    property color text_field_placeholder_color: "#938f99"
    property color text_field_color: "transparent"
    property color text_field_border_color: "transparent"
    property real text_field_border_width: 0
    property real text_field_radius: 0

    visible: true
    color: "transparent"

    width: launcher ? launcher.windowWidth : 500
    height: root.contentHeight()

    function contentHeight() {
        var count = launcher ? launcher.apps.length : 0
        var effectiveMin = Math.min(Math.max(0, min_visible_items), Math.max(0, max_visible_items))
        var visibleItems = count > 0 ? Math.min(Math.max(effectiveMin, count), max_visible_items) : effectiveMin
        var h = content_margins * 2

        var hasTopPrompt = root.prompt_exists && root.prompt_text === "top"
        var hasEntryPrompt = root.prompt_exists && root.prompt_text === "entry"
        if (hasTopPrompt) h += 30 + content_spacing
        if (root.show_text_field || hasEntryPrompt) h += 50 + content_spacing

        var listHeight = 0
        if (visibleItems > 0) {
            for (var i = 0; i < visibleItems; ++i) {
                var app = i < count ? launcher.apps[i] : null
                listHeight += (app && app.description) ? item_height_description : item_height
            }
            listHeight += Math.max(0, visibleItems - 1) * item_spacing
        }

        return h + listHeight
    }

    LayerShellQt.Window.layer: LayerShellQt.Window.LayerOverlay
    LayerShellQt.Window.keyboardInteractivity: LayerShellQt.Window.KeyboardInteractivityExclusive
    LayerShellQt.Window.anchors: LayerShellQt.Window.AnchorNone
    LayerShellQt.Window.wantsToBeOnActiveScreen: true

    readonly property bool prompt_exists: launcher && launcher.promptText !== ""
    readonly property string prompt_text: prompt_exists ? launcher.promptPositionText : ""

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
        scale: 0.5
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
                text: launcher ? launcher.promptText : ""
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
                        text: launcher ? launcher.promptText : ""
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
                    placeholderText: launcher ? launcher.placeholder : "Search..."
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
                    var effectiveMin = Math.min(Math.max(0, root.min_visible_items), Math.max(0, root.max_visible_items))
                    var visibleItems = count > 0 ? Math.min(Math.max(effectiveMin, count), root.max_visible_items) : effectiveMin
                    var total = 0
                    for (var i = 0; i < visibleItems; ++i) {
                        var app = i < count ? launcher.apps[i] : null
                        total += (app && app.description) ? root.item_height_description : root.item_height
                    }
                    return total + Math.max(0, visibleItems - 1) * root.item_spacing
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
                                    font.pixelSize: root.font_size
                                    color: root.text_color
                                }

                                Text {
                                    visible: modelData.description && modelData.description !== ""
                                    text: modelData.description
                                    width: parent.width
                                    elide: Text.ElideRight
                                    maximumLineCount: 1
                                    clip: true
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

    function resetAnimation() {
        closeAnimation.stop()
        closing = false
        container.opacity = 1
        container.scale = 1
    }

    Component.onCompleted: {
        if (!preload_mode) {
            container.opacity = 1
            container.scale = 1
        }

        Qt.callLater(function() {
            keyboardFocus.forceActiveFocus()
            if (root.show_text_field) {
                search.forceActiveFocus()
            }
        })
    }

    onVisibleChanged: {
        if (preload_mode) {
            search.text = ""
            list.currentIndex = 0
            list.hoveredIndex = -1
        }

        if (visible) {
            keyboardFocus.forceActiveFocus()
            if (root.show_text_field) {
                search.forceActiveFocus()
            }
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: {
            root.closeAnimated()
        }
    }

    ParallelAnimation {
        id: closeAnimation

        NumberAnimation {
            target: container
            property: "opacity"
            to: 0
            duration: 140
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            target: container
            property: "scale"
            to: 0.5
            duration: 180
            easing.type: Easing.OutCubic
        }

        onFinished: {
            if (typeof preload_mode !== "undefined" && preload_mode) {
                root.hide()
            } else {
                root.close()
            }
        }
    }

    function closeAnimated() {
        if (closing) return
        closing = true
        closeAnimation.start()
    }

    onClosing: {
        if (preload_mode) {
            close.accepted = false
            hide()
        }
    }
}
