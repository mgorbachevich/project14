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
    property bool capital: false

    Column
    {
        id: virtualKeyboardLatinLayout
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: capital ? qsTr("Q") : qsTr("q") }
            KeyboardCharButton { text: capital ? qsTr("W") : qsTr("w") }
            KeyboardCharButton { text: capital ? qsTr("E") : qsTr("e") }
            KeyboardCharButton { text: capital ? qsTr("R") : qsTr("r") }
            KeyboardCharButton { text: capital ? qsTr("T") : qsTr("t") }
            KeyboardCharButton { text: capital ? qsTr("Y") : qsTr("y") }
            KeyboardCharButton { text: capital ? qsTr("U") : qsTr("u") }
            KeyboardCharButton { text: capital ? qsTr("I") : qsTr("i") }
            KeyboardCharButton { text: capital ? qsTr("O") : qsTr("o") }
            KeyboardCharButton { text: capital ? qsTr("P") : qsTr("p") }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: capital ? qsTr("A") : qsTr("a") }
            KeyboardCharButton { text: capital ? qsTr("S") : qsTr("s") }
            KeyboardCharButton { text: capital ? qsTr("D") : qsTr("d") }
            KeyboardCharButton { text: capital ? qsTr("F") : qsTr("f") }
            KeyboardCharButton { text: capital ? qsTr("G") : qsTr("g") }
            KeyboardCharButton { text: capital ? qsTr("H") : qsTr("h") }
            KeyboardCharButton { text: capital ? qsTr("J") : qsTr("j") }
            KeyboardCharButton { text: capital ? qsTr("K") : qsTr("k") }
            KeyboardCharButton { text: capital ? qsTr("L") : qsTr("l") }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardIconButton
            {
                icon.source: "../Icons/arrow_up_black_48"
                onClicked: { app.onUserAction(); capital = !capital }
            }
            KeyboardCharButton { text: capital ? qsTr("Z") : qsTr("z") }
            KeyboardCharButton { text: capital ? qsTr("X") : qsTr("x") }
            KeyboardCharButton { text: capital ? qsTr("C") : qsTr("c") }
            KeyboardCharButton { text: capital ? qsTr("V") : qsTr("v") }
            KeyboardCharButton { text: capital ? qsTr("B") : qsTr("b") }
            KeyboardCharButton { text: capital ? qsTr("N") : qsTr("n") }
            KeyboardCharButton { text: capital ? qsTr("M") : qsTr("m") }
            KeyboardIconButton
            {
                icon.source: "../Icons/backspace_black_48";
                onClicked: keyEmitter.emitKey(Qt.Key_Backspace)
            }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardButton
            {
                text: qsTr("1.,")
                width: screenManager.buttonSize() * 3 / 2
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(2)
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
                icon.source: "../Icons/ok_outline_black_48";
                onClicked: keyEmitter.emitKey(Qt.Key_Return)
            }
            */
        }
    }
}
