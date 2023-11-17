import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Rectangle
{
    property bool compact: false
    property bool numeric: false
    property bool latin: true
    property bool capital: false
    width: parent.width
    height: compact ? app.buttonSize() * 2 + app.spacer() * 3 / 2 : app.buttonSize() * 4 + app.spacer() * 5 / 2
    color: Material.color(Material.Grey, Material.Shade100)

    Column
    {
        id: virtualKeyboardCompactLayout
        visible: compact
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: qsTr("1"); onClicked: keyEmitter.emitKey(Qt.Key_1) }
            KeyboardTextButton { text: qsTr("2"); onClicked: keyEmitter.emitKey(Qt.Key_2) }
            KeyboardTextButton { text: qsTr("3"); onClicked: keyEmitter.emitKey(Qt.Key_3) }
            KeyboardTextButton { text: qsTr("4"); onClicked: keyEmitter.emitKey(Qt.Key_4) }
            KeyboardTextButton { text: qsTr("5"); onClicked: keyEmitter.emitKey(Qt.Key_5) }
            KeyboardTextButton { text: qsTr("6"); onClicked: keyEmitter.emitKey(Qt.Key_6) }
            KeyboardTextButton { text: qsTr("7"); onClicked: keyEmitter.emitKey(Qt.Key_7) }
            KeyboardTextButton { text: qsTr("8"); onClicked: keyEmitter.emitKey(Qt.Key_8) }
            KeyboardTextButton { text: qsTr("9"); onClicked: keyEmitter.emitKey(Qt.Key_9) }
            KeyboardTextButton { text: qsTr("0"); onClicked: keyEmitter.emitKey(Qt.Key_0) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardIconButton { icon.source: "../Icons/keyboard_black_48"; onClicked: {} }
            KeyboardTextButton { text: qsTr(" "); width: app.buttonSize() * 5; onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/backspace_black_48"; onClicked: {} }
        }
    }

    Column
    {
        id: virtualKeyboardNumericLayout
        visible: !compact & numeric
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: qsTr("1"); onClicked: keyEmitter.emitKey(Qt.Key_1) }
            KeyboardTextButton { text: qsTr("2"); onClicked: keyEmitter.emitKey(Qt.Key_2) }
            KeyboardTextButton { text: qsTr("3"); onClicked: keyEmitter.emitKey(Qt.Key_3) }
            KeyboardTextButton { text: qsTr("4"); onClicked: keyEmitter.emitKey(Qt.Key_4) }
            KeyboardTextButton { text: qsTr("5"); onClicked: keyEmitter.emitKey(Qt.Key_5) }
            KeyboardTextButton { text: qsTr("6"); onClicked: keyEmitter.emitKey(Qt.Key_6) }
            KeyboardTextButton { text: qsTr("7"); onClicked: keyEmitter.emitKey(Qt.Key_7) }
            KeyboardTextButton { text: qsTr("8"); onClicked: keyEmitter.emitKey(Qt.Key_8) }
            KeyboardTextButton { text: qsTr("9"); onClicked: keyEmitter.emitKey(Qt.Key_9) }
            KeyboardTextButton { text: qsTr("0"); onClicked: keyEmitter.emitKey(Qt.Key_0) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: qsTr("-"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("/"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr(":"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr(";"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("("); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr(")"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("№"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("@"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("\""); onClicked: keyEmitter.emitChar(text) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: qsTr("%"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("*"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("_"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("."); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr(","); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("?"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("!"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: qsTr("'"); onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/backspace_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Backspace) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton
            {
                text: qsTr("LAT")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); numeric = false; latin = true }
            }
            KeyboardTextButton
            {
                text: qsTr("РУС")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); numeric = false; latin = false }
            }
            KeyboardTextButton { text: qsTr(" "); width: app.buttonSize() * 4; onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/ok_outline_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Return) }
        }
    }

    Column
    {
        id: virtualKeyboardLatinLayout
        visible: !compact & !numeric & latin
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: capital ? qsTr("Q") : qsTr("q"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("W") : qsTr("w"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("E") : qsTr("e"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("R") : qsTr("r"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("T") : qsTr("t"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Y") : qsTr("y"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("U") : qsTr("u"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("I") : qsTr("i"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("O") : qsTr("o"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("P") : qsTr("p"); onClicked: keyEmitter.emitChar(text) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: capital ? qsTr("A") : qsTr("a"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("S") : qsTr("s"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("D") : qsTr("d"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("F") : qsTr("f"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("G") : qsTr("g"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("H") : qsTr("h"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("J") : qsTr("j"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("K") : qsTr("k"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("L") : qsTr("l"); onClicked: keyEmitter.emitChar(text) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardIconButton
            {
                icon.source: "../Icons/arrow_up_black_48"
                onClicked: { app.onUserAction(); capital = !capital }
            }
            KeyboardTextButton { text: capital ? qsTr("Z") : qsTr("z"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("X") : qsTr("x"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("C") : qsTr("c"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("V") : qsTr("v"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("B") : qsTr("b"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("N") : qsTr("n"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("M") : qsTr("m"); onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/backspace_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Backspace) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton {
                text: qsTr("1.,")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); numeric = true }
            }
            KeyboardTextButton
            {
                text: qsTr("РУС")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); latin = false }
            }
            KeyboardTextButton { text: qsTr(" "); width: app.buttonSize() * 4; onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/ok_outline_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Return) }
        }
    }

    Column
    {
        id: virtualKeyboardCyrillicLayout
        visible: !compact & !numeric & !latin
        anchors.horizontalCenter: parent.horizontalCenter

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: capital ? qsTr("Й") : qsTr("й"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ц") : qsTr("ц"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("У") : qsTr("у"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("К") : qsTr("к"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Е") : qsTr("е"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Н") : qsTr("н"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Г") : qsTr("г"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ш") : qsTr("ш"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Щ") : qsTr("щ"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("З") : qsTr("з"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Х") : qsTr("х"); onClicked: keyEmitter.emitChar(text) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton { text: capital ? qsTr("Ф") : qsTr("ф"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ы") : qsTr("ы"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("В") : qsTr("в"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("А") : qsTr("а"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("П") : qsTr("п"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Р") : qsTr("р"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("О") : qsTr("о"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Л") : qsTr("л"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Д") : qsTr("д"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ж") : qsTr("ж"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Э") : qsTr("э"); onClicked: keyEmitter.emitChar(text) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardIconButton
            {
                icon.source: "../Icons/arrow_up_black_48"
                onClicked: { app.onUserAction(); capital = !capital }
            }
            KeyboardTextButton { text: capital ? qsTr("Я") : qsTr("я"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ч") : qsTr("ч"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("С") : qsTr("с"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("М") : qsTr("м"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("И") : qsTr("и"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Т") : qsTr("т"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ь") : qsTr("ь"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Б") : qsTr("б"); onClicked: keyEmitter.emitChar(text) }
            KeyboardTextButton { text: capital ? qsTr("Ю") : qsTr("ю"); onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/backspace_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Backspace) }
        }

        Row
        {
            anchors.horizontalCenter: parent.horizontalCenter

            KeyboardTextButton {
                text: qsTr("1.,")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); numeric = true }
            }
            KeyboardTextButton
            {
                text: qsTr("LAT")
                width: app.buttonSize() * 3 / 2
                font { pointSize: app.normalFontSize() }
                onClicked: { app.onUserAction(); latin = true }
            }
            KeyboardTextButton { text: qsTr(" "); width: app.buttonSize() * 4; onClicked: keyEmitter.emitChar(text) }
            KeyboardIconButton { icon.source: "../Icons/ok_outline_black_48"; onClicked: keyEmitter.emitKey(Qt.Key_Return) }
        }
    }
}
