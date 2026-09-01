import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: root

    required property bool showTextField
    required property bool showEntryPrompt
    required property string promptText
    required property string placeholderText
    required property color textColor
    required property color placeholderColor
    required property color fieldColor
    required property color fieldBorderColor
    required property real fieldBorderWidth
    required property real fieldRadius
    required property int fontSize
    required property bool fontBold

    property alias text: search.text

    signal textChangedByUser(string text)

    spacing: 10
    width: parent ? parent.width : 0
    height: visible ? 50 : 0
    visible: root.showTextField || root.showEntryPrompt

    Text {
        visible: root.showEntryPrompt
        text: root.promptText
        color: root.textColor
        font.pixelSize: 18
    }

    TextField {
        id: search

        Layout.fillWidth: true
        Layout.preferredHeight: 50
        visible: root.showTextField
        enabled: root.showTextField
        focus: true
        placeholderText: root.placeholderText
        color: root.textColor
        placeholderTextColor: root.placeholderColor
        font.pixelSize: root.fontSize
        font.bold: root.fontBold

        background: Rectangle {
            color: root.fieldColor
            border.color: root.fieldBorderColor
            border.width: root.fieldBorderWidth
            radius: root.fieldRadius
        }

        onTextChanged: root.textChangedByUser(text)
    }
}
