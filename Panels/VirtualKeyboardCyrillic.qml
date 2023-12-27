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
        id: virtualKeyboardCyrillicLayout
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: capital ? qsTr("Й") : qsTr("й") }
            KeyboardCharButton { text: capital ? qsTr("Ц") : qsTr("ц") }
            KeyboardCharButton { text: capital ? qsTr("У") : qsTr("у") }
            KeyboardCharButton { text: capital ? qsTr("К") : qsTr("к") }
            KeyboardCharButton { text: capital ? qsTr("Е") : qsTr("е") }
            KeyboardCharButton { text: capital ? qsTr("Н") : qsTr("н") }
            KeyboardCharButton { text: capital ? qsTr("Г") : qsTr("г") }
            KeyboardCharButton { text: capital ? qsTr("Ш") : qsTr("ш") }
            KeyboardCharButton { text: capital ? qsTr("Щ") : qsTr("щ") }
            KeyboardCharButton { text: capital ? qsTr("З") : qsTr("з") }
            KeyboardCharButton { text: capital ? qsTr("Х") : qsTr("х") }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardCharButton { text: capital ? qsTr("Ф") : qsTr("ф") }
            KeyboardCharButton { text: capital ? qsTr("Ы") : qsTr("ы") }
            KeyboardCharButton { text: capital ? qsTr("В") : qsTr("в") }
            KeyboardCharButton { text: capital ? qsTr("А") : qsTr("а") }
            KeyboardCharButton { text: capital ? qsTr("П") : qsTr("п") }
            KeyboardCharButton { text: capital ? qsTr("Р") : qsTr("р") }
            KeyboardCharButton { text: capital ? qsTr("О") : qsTr("о") }
            KeyboardCharButton { text: capital ? qsTr("Л") : qsTr("л") }
            KeyboardCharButton { text: capital ? qsTr("Д") : qsTr("д") }
            KeyboardCharButton { text: capital ? qsTr("Ж") : qsTr("ж") }
            KeyboardCharButton { text: capital ? qsTr("Э") : qsTr("э") }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardIconButton
            {
                id: shiftButton
                icon.source: "../Icons/arrow_up"
                onClicked: { app.onUserAction(); capital = !capital; }
            }
            KeyboardCharButton { text: capital ? qsTr("Я") : qsTr("я") }
            KeyboardCharButton { text: capital ? qsTr("Ч") : qsTr("ч") }
            KeyboardCharButton { text: capital ? qsTr("С") : qsTr("с") }
            KeyboardCharButton { text: capital ? qsTr("М") : qsTr("м") }
            KeyboardCharButton { text: capital ? qsTr("И") : qsTr("и") }
            KeyboardCharButton { text: capital ? qsTr("Т") : qsTr("т") }
            KeyboardCharButton { text: capital ? qsTr("Ь") : qsTr("ь") }
            KeyboardCharButton { text: capital ? qsTr("Б") : qsTr("б") }
            KeyboardCharButton { text: capital ? qsTr("Ю") : qsTr("ю") }
            KeyboardIconButton
            {
                icon.source: "../Icons/backspace";
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
                text: qsTr("LAT")
                width: screenManager.buttonSize() * 3 / 2
                font { pointSize: screenManager.normalFontSize() }
                onClicked: app.onVirtualKeyboardSet(0)
            }
            KeyboardCharButton { text: qsTr(" "); width: screenManager.buttonSize() * 4 }
            /*
            KeyboardIconButton
            {
                icon.source: "../Icons/ok_outline";
                onClicked: keyEmitter.emitKey(Qt.Key_Return)
            }
            */
        }
    }
}
