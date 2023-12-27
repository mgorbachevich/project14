import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    // width: parent.width
    height: screenManager.buttonSize() * 4
    color: Material.color(Material.Grey, Material.Shade100)
    function emitKey(v) { keyEmitter.emitKey(v) }

    Column
    {
        id: virtualKeyboardNumericLayout
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardButton { text: qsTr("1"); onClicked: emitKey(Qt.Key_1) }
            KeyboardButton { text: qsTr("2"); onClicked: emitKey(Qt.Key_2) }
            KeyboardButton { text: qsTr("3"); onClicked: emitKey(Qt.Key_3) }
            KeyboardButton { text: qsTr("4"); onClicked: emitKey(Qt.Key_4) }
            KeyboardButton { text: qsTr("5"); onClicked: emitKey(Qt.Key_5) }
            KeyboardButton { text: qsTr("6"); onClicked: emitKey(Qt.Key_6) }
            KeyboardButton { text: qsTr("7"); onClicked: emitKey(Qt.Key_7) }
            KeyboardButton { text: qsTr("8"); onClicked: emitKey(Qt.Key_8) }
            KeyboardButton { text: qsTr("9"); onClicked: emitKey(Qt.Key_9) }
            KeyboardButton { text: qsTr("0"); onClicked: emitKey(Qt.Key_0) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: qsTr("-") }
            KeyboardCharButton { text: qsTr("/") }
            KeyboardCharButton { text: qsTr(":") }
            KeyboardCharButton { text: qsTr(";") }
            KeyboardCharButton { text: qsTr("(") }
            KeyboardCharButton { text: qsTr(")") }
            KeyboardCharButton { text: qsTr("№") }
            KeyboardCharButton { text: qsTr("@") }
            KeyboardCharButton { text: qsTr("\"") }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: qsTr("%") }
            KeyboardCharButton { text: qsTr("*") }
            KeyboardCharButton { text: qsTr("_") }
            KeyboardCharButton { text: qsTr(".") }
            KeyboardCharButton { text: qsTr(",") }
            KeyboardCharButton { text: qsTr("?") }
            KeyboardCharButton { text: qsTr("!") }
            KeyboardCharButton { text: qsTr("'") }
            KeyboardIconButton
            {
                icon.source: "../Icons/backspace";
                onClicked: emitKey(Qt.Key_Backspace)
            }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardButton
            {
                text: qsTr("LAT")
                width: screenManager.buttonSize() * 3 / 2
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(0)
            }
            KeyboardButton
            {
                text: qsTr("РУС")
                width: screenManager.buttonSize() * 3 / 2
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(1)
            }
            KeyboardCharButton { text: qsTr(" "); width: screenManager.buttonSize() * 4 }
            /*
            KeyboardIconButton
            {
                icon.source: "../Icons/ok_outline";
                onClicked: emitKey(Qt.Key_Return)
            }
            */
        }
    }
}
