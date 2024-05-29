import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    height: screenManager.keyboardHeight()
    color: Material.color(Material.Grey, Material.Shade100)
    function emitKey(v) { keyEmitter.emitKey(v) }

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
        }

        Column
        {
            KeyboardButton
            {
                text: qsTr("LAT")
                width: screenManager.buttonSize() * 4 / 3
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(0)
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
