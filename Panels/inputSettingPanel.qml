import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: inputSettingPanel
    padding : 0
    margins: 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    property string titleText: "Title"
    property string inputText: "Input"
    property int settingItemCode: 0
    property int virtualKeyboardSet: 2
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        app.onSettingInputClosed(settingItemCode, inputSettingPanelText.text)
        app.onPopupOpened(false)
    }

    Connections // Slot for signal KeyEmitter::enterChar
    {
        target: keyEmitter
        function onEnterChar(value)
        {
            //app.debugLog("@@@@@ inputSettingPanel onEnterChar %1".arg(value))
            inputSettingPanelText.text += value
        }
    }

    Connections // Slot for signal AppManager::showVirtualKeyboard
    {
        target: app
        function onShowVirtualKeyboard(value)
        {
            //app.debugLog("@@@@@ inputSettingPanel onShowVirtualKeyboard %1".arg(value))
            virtualKeyboardSet = value
        }
    }

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columns: 3
        rows: 3

        EmptyButton
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: parent
            color: "transparent"

            CardTitleText { text: titleText }
        }

        RoundIconButton
        {
            Layout.column: 2
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            icon.source: "../Icons/close"
            onClicked:
            {
                app.onUserAction();
                inputSettingPanel.close()
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 1
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            Layout.bottomMargin: screenManager.buttonSize() * 3 / 4
            color: "transparent"

            TextField
            {
                id: inputSettingPanelText
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                width: screenManager.editWidth()
                font { pointSize: screenManager.largeFontSize() }
                Material.accent: Material.Orange
                color: Material.color(Material.BlueGrey, Material.Shade900)
                focus: true
                text: inputText

                Keys.onPressed: (event) =>
                {
                    //app.debugLog("@@@@@ inputSettingPanelText Keys.onPressed %1".arg(JSON.stringify(event)))
                    event.accepted = true;
                    app.clickSound()
                    app.onUserAction();
                    switch (event.key)
                    {
                        case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                        case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                            text += event.text
                            break;
                        case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                            text = text.substring(0, text.length - 1);
                            break;
                        case Qt.Key_Escape:
                            text = ""
                            break;
                        case Qt.Key_Enter: case Qt.Key_Return:
                            inputSettingPanel.close()
                            break
                        case Qt.Key_F10: // Промотка
                            app.onRewind()
                            break
                        default:
                            app.beepSound();
                            break
                    }
                }
            }
        }

        Loader
        {
            Layout.column: 1
            Layout.row: 2
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.fillWidth: parent
            focus: false
            source: virtualKeyboardSet === 0 ? "VirtualKeyboardLatin.qml" :
                    virtualKeyboardSet === 1 ? "VirtualKeyboardCyrillic.qml" :
                                               "VirtualKeyboardNumeric.qml"
        }
    }
}

