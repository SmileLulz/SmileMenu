import QtQuick
// import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
// import QtQuick.Effects
import org.kde.layershell as LayerShellQt
import "."

Window {
    id: root

    property bool closing: false

    property real window_radius: 40
    property color background_color: "#1c192b"

    property real item_spacing: 0
    property real item_container_radius: 28
    property real item_radius_high: 28
    property real item_radius_low: 0
    property color item_color: "transparent"
    property color item_hover_color: "#484459"
    property color item_container_color: "#312e41"
    property int item_height: 50
    property int item_height_description: 65

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
    width: Api.windowWidth
    height: root.contentHeight()

    function contentHeight() {
        var count = Api.apps.length
        var effectiveMin = Math.min(
            Math.max(0, Api.minVisibleItems),
            Math.max(0, Api.maxVisibleItems)
        )
        var visibleItems = count > 0
            ? Math.min(Math.max(effectiveMin, count), Api.maxVisibleItems)
            : effectiveMin

        var h = root.content_margins * 2
        var hasTopPrompt = root.prompt_exists && root.prompt_text === "top"
        var hasEntryPrompt = root.prompt_exists && root.prompt_text === "entry"

        if (hasTopPrompt)
            h += 30 + root.content_spacing

        if (Api.showTextField || hasEntryPrompt)
            h += 50 + root.content_spacing

        var listHeight = 0
        if (visibleItems > 0) {
            for (var i = 0; i < visibleItems; ++i) {
                var app = i < count ? Api.apps[i] : null
                listHeight += (app && app.description)
                    ? root.item_height_description
                    : root.item_height
            }
            listHeight += Math.max(0, visibleItems - 1) * root.item_spacing
        }

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

        Keys.onDownPressed: menu.moveSelection(1)
        Keys.onUpPressed: menu.moveSelection(-1)

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
            NumberAnimation { duration: 140; easing.type: Easing.OutCubic }
        }

        Behavior on scale {
            NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
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
                    var total = 0

                    for (var i = 0; i < visibleItems; ++i) {
                        var app = i < count ? Api.apps[i] : null
                        total += (app && app.description)
                            ? root.item_height_description
                            : root.item_height
                    }

                    return total + Math.max(0, visibleItems - 1) * root.item_spacing
                }

                ItemList {
                    id: menu
                    anchors.fill: parent
                    model: Api.apps
                    cycle: Api.cycle
                    itemSpacing: root.item_spacing
                    itemContainerRadius: root.item_container_radius
                    itemColor: root.item_color
                    itemHoverColor: root.item_hover_color
                    itemContainerColor: root.item_container_color
                    itemHeight: root.item_height
                    itemHeightDescription: root.item_height_description
                    textColor: root.text_color
                    textColorSecondary: root.text_color_secondary
                    fontSize: root.font_size
                    fontSizeSecondary: root.font_size_secondary
                    fontBold: root.font_bold
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

        Qt.callLater(function() {
            keyboardFocus.forceActiveFocus()
            if (Api.showTextField)
                searchField.forceActiveFocus()
        })
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
                searchField.forceActiveFocus()
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
        if (closing)
            return
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
