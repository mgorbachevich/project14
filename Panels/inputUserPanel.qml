import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: inputUserPanel
    padding : 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    property int virtualKeyboardSet: 2
    property string userCode: ""
    property string userName: ""
    property string userPassword: ""
    property bool isUserAdmin: false
    onOpened:
    {
        app.onPopupOpened(true)
        userCodeTextField.focus = true
    }
    onClosed: app.onPopupOpened(false)

    Connections // Slot for signal AppManager::showVirtualKeyboard
    {
        target: app
        function onShowVirtualKeyboard(value)
        {
            //app.debugLog("@@@@@ inputUserPanel onShowVirtualKeyboard %1".arg(value))
            virtualKeyboardSet = value
        }
    }

    Connections // Slot for signal KeyEmitter::enterChar
    {
        target: keyEmitter
        function onEnterChar(value)
        {
            //app.debugLog("@@@@@ inputUserPanel onEnterChar %1".arg(value))
            if(userCodeTextField.activeFocus) userCodeTextField.text += value
            if(userNameTextField.activeFocus) userNameTextField.text += value
            if(userPasswordTextField.activeFocus) userPasswordTextField.text += value
        }
    }

    GridLayout
    {
        id: inputUserPanelLayout
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columnSpacing: screenManager.spacer() * 2

        RoundIconButton
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            icon.source: "../Icons/delete"
            onClicked:
            {
                app.onDeleteUserClicked(userCode)
                inputUserPanel.close()
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.columnSpan: 4
            Layout.fillWidth: parent
            color: "transparent"

            CardTitleText { text: "Пользователь" }
        }

        RoundIconButton
        {
            Layout.column: 5
            Layout.row: 0
            icon.source: "../Icons/close"
            Layout.alignment: Qt.AlignTop | Qt.AlignRigth
            onClicked:
            {
                app.onInputUserClosed(userCodeTextField.text, userNameTextField.text, userPasswordTextField.text, userAdminCheckBox.checked)
                inputUserPanel.close()
            }
        }

        SubtitleText
        {
            Layout.column: 1
            Layout.row: 1
            text: qsTr("Код")
        }

        TextField
        {
            id: userCodeTextField
            Layout.column: 1
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.fillWidth: parent
            font { pointSize: screenManager.normalFontSize() }
            Material.accent: Material.Orange
            color: Material.color(Material.BlueGrey, Material.Shade900)
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            text: userCode
        }

        SubtitleText
        {
            Layout.column: 2
            Layout.row: 1
            text: qsTr("Имя")
        }

        TextField
        {
            id: userNameTextField
            Layout.column: 2
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.fillWidth: parent
            font { pointSize: screenManager.normalFontSize() }
            Material.accent: Material.Orange
            color: Material.color(Material.BlueGrey, Material.Shade900)
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            text: userName
        }

        SubtitleText
        {
            Layout.column: 3
            Layout.row: 1
            text: qsTr("Пароль")
        }

        TextField
        {
            id: userPasswordTextField
            Layout.column: 3
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            Layout.fillWidth: parent
            font { pointSize: screenManager.normalFontSize() }
            Material.accent: Material.Orange
            color: Material.color(Material.BlueGrey, Material.Shade900)
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            text: userPassword
        }

        SubtitleText
        {
            Layout.column: 4
            Layout.row: 1
            text: qsTr("Админ")
        }

        CheckBox
        {
            id: userAdminCheckBox
            Layout.column: 4
            Layout.row: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            checked: isUserAdmin
        }

        Loader
        {
            Layout.column: 0
            Layout.row: 3
            Layout.columnSpan: 6
            Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
            Layout.fillWidth: parent
            focus: false
            source: virtualKeyboardSet === 0 ? "VirtualKeyboardLatin.qml" :
                    virtualKeyboardSet === 1 ? "VirtualKeyboardCyrillic.qml" :
                                               "VirtualKeyboardNumeric.qml"
        }
    }
}


