import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    height: screenManager.keyboardHeight()
    color: Material.color(Material.Grey, Material.Shade100)
    property bool capital: false

    Row
    {
        anchors.horizontalCenter: parent.horizontalCenter

        Column
        {
            visible: app.isAdmin() && !app.isAuthorizationOpened() && !app.isSettingsOpened()
            KeyboardIconButton
            {
                icon.source: "../Icons/settings";
                marked: true
                onClicked: app.onAdminSettingsClicked()
            }
            KeyboardIconButton
            {
                icon.source: "../Icons/lock"
                marked: true
                onClicked: app.onLockClicked()
            }
            KeyboardIconButton
            {
                icon.source: "../Icons/log"
                marked: true
                onClicked: app.onViewLogClicked()
            }
        }

        Column
        {
            width: screenManager.buttonSize() * 11

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
                    icon.source: "../Icons/arrow_up"
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
                    icon.source: "../Icons/backspace";
                    onClicked: keyEmitter.emitKey(Qt.Key_Backspace)
                }
            }
        }

        Column
        {
            KeyboardButton
            {
                text: qsTr("1.,")
                width: screenManager.buttonSize() * 4 / 3
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(2)
            }
            KeyboardButton
            {
                text: qsTr("РУС")
                width: screenManager.buttonSize() * 4 / 3
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(1)
            }
            KeyboardCharButton
            {
                text: qsTr(" ");
                width: screenManager.buttonSize() * 4 / 3
            }
        }
    }
}
