import QtQuick
import QtQuick.Controls
import QtQuick.Window
import org.kde.layershell as LayerShellQt
import "."

Window {
    id: root

    property bool closing: false

    property real window_radius: Theme.sizes.windowRadius
    property color background_color: Theme.colors.background

    property int content_margins: Theme.sizes.contentMargins
    property int content_spacing: Theme.sizes.contentSpacing

    property color text_color: Theme.colors.text
    property color text_color_secondary: Theme.colors.textSecondary
    property int font_size: Theme.sizes.fontSize
    property int font_size_secondary: Theme.sizes.fontSizeSecondary
    property bool font_bold: Theme.sizes.fontBold

    property color text_field_placeholder_color: Theme.colors.placeholder
    property color text_field_color: Theme.colors.fieldBackground
    property color text_field_border_color: Theme.colors.fieldBorder
    property real text_field_border_width: Theme.sizes.fieldBorderWidth
    property real text_field_radius: Theme.sizes.fieldRadius

    visible: true
    color: "transparent"

    width: Api.windowWidth
    height: root.contentHeight()

    function contentHeight() {
        var count = Api.apps.length

        var effectiveMin = Math.min(
            Math.max(0, Api.minVisibleItems),
            Math.max(0, Api.maxVisibleItems)
        )

        var visibleItems = count > 0
            ? Math.min(
                Math.max(effectiveMin, count),
                Api.maxVisibleItems
            )
            : effectiveMin

        var h = root.content_margins * 2

        var hasTopPrompt =
            root.prompt_exists &&
            root.prompt_text === "top"

        var hasEntryPrompt =
            root.prompt_exists &&
            root.prompt_text === "entry"

        if (hasTopPrompt)
            h += 30 + root.content_spacing

        if (Api.showTextField || hasEntryPrompt)
            h += 50 + root.content_spacing

        var columns = Math.min(
            4,
            Math.max(1, visibleItems)
        )

        var rows = visibleItems > 0
            ? Math.ceil(visibleItems / columns)
            : 0

        var listHeight = rows * Theme.sizes.cellHeight

        return h + listHeight
    }

    LayerShellQt.Window.layer: LayerShellQt.Window.LayerOverlay
    LayerShellQt.Window.keyboardInteractivity: LayerShellQt.Window.KeyboardInteractivityExclusive
    LayerShellQt.Window.anchors: LayerShellQt.Window.AnchorNone
    LayerShellQt.Window.wantsToBeOnActiveScreen: true

    readonly property bool prompt_exists: Api.promptText !== ""
    readonly property string prompt_text: prompt_exists ? Api.promptPositionText : ""

    FocusScope {
        id: keyboardFocus

        anchors.fill: parent
        focus: true

        Keys.onDownPressed: menu.moveVertical(1)
        Keys.onUpPressed: menu.moveVertical(-1)
        Keys.onLeftPressed: menu.moveHorizontal(-1)
        Keys.onRightPressed: menu.moveHorizontal(1)
        Keys.onReturnPressed: {
            if (menu.currentIndex >= 0 && menu.currentIndex < Api.apps.length) {
                Api.launch(Api.apps[menu.currentIndex].command)
                root.closeAnimated()
            }
        }
        Keys.onEscapePressed: root.closeAnimated()
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
                text: Api.promptText
                color: root.text_color
                font.pixelSize: 24
            }

            SearchField {
                id: searchField

                showTextField: Api.showTextField
                showEntryPrompt: root.prompt_exists && root.prompt_text === "entry"
                promptText: Api.promptText
                placeholderText: Api.placeholder
                textColor: root.text_color
                placeholderColor: root.text_field_placeholder_color
                fieldColor: root.text_field_color
                fieldBorderColor: root.text_field_border_color
                fieldBorderWidth: root.text_field_border_width
                fieldRadius: root.text_field_radius
                fontSize: root.font_size
                fontBold: root.font_bold

                Keys.forwardTo: [keyboardFocus]

                onTextChangedByUser: {
                    Api.search(text)
                    menu.currentIndex = 0
                    menu.hoveredIndex = -1
                }
            }

            Item {
                width: parent.width
                height: {
                    var count = Api.apps.length
                    var effectiveMin = Math.min(
                        Math.max(0, Api.minVisibleItems),
                        Math.max(0, Api.maxVisibleItems)
                    )
                    var visibleItems = count > 0
                        ? Math.min(Math.max(effectiveMin, count), Api.maxVisibleItems)
                        : effectiveMin
                    var columns = Math.min(4, Math.max(1, visibleItems))
                    var rows = visibleItems > 0 ? Math.ceil(visibleItems / columns) : 0
                    return rows * Theme.sizes.cellHeight
                }

                ItemList {
                    id: menu

                    anchors.fill: parent
                    model: Api.apps
                    cycle: Api.cycle

                    onActivated: function(command) {
                        Api.launch(command)
                        root.closeAnimated()
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
    }

    onVisibleChanged: {
        if (preload_mode) {
            searchField.text = ""
            menu.currentIndex = 0
            menu.hoveredIndex = -1
        }
        if (visible) {
            keyboardFocus.forceActiveFocus()
            if (Api.showTextField)
                searchField.field.forceActiveFocus()
        }
    }

    Shortcut {
        sequence: "Escape"
        onActivated: root.closeAnimated()
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
            if (typeof preload_mode !== "undefined" && preload_mode)
                root.hide()
            else
                root.close()
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
