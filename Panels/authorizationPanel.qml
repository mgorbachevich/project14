import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: authorizationPanel
    padding: screenManager.spacer()
    closePolicy: Popup.NoAutoClose
    focus: true
    modal: true
    dim: true
    Material.background: Material.color(Material.Grey, Material.Shade100)
    property string versionValue: "Version"
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::authorizationSucceded:
    {
        target: app
        function onAuthorizationSucceded()
        {
            app.onUserAction();
            authorizationPanel.close()
        }
    }

    Connections // Slot for signal AppManager::setCurrentUser:
    {
        target: app
        function onSetCurrentUser(index, name)
        {
            loginComboBox.currentIndex = index
            loginComboBox.displayText = name
        }
    }

    GridLayout
    {
        id: authorizationPanelLayout
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        //columns: 1

        Text
        {
            id: versionText
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: Qt.AlignHCenter
            color: Material.color(Material.BlueGrey, Material.Shade600)
            font { pointSize: screenManager.normalFontSize() }
            text: versionValue
        }

        Text
        {
            Layout.column: 0
            Layout.row: 1
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: screenManager.spacer()
            font { pointSize: screenManager.largeFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade900)
            text: qsTr("Авторизация")
        }

        Text
        {
            Layout.column: 0
            Layout.row: 2
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: screenManager.spacer()
            font { pointSize: screenManager.normalFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: qsTr("Пользователь")
        }

        ComboBox
        {
            id: loginComboBox
            Layout.column: 0
            Layout.row: 3
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: screenManager.editWidth()
            editable: false
            popup.modal: true

            model: userNameModel
            delegate: Text
            {
                padding: screenManager.spacer()
                font { pointSize: screenManager.normalFontSize() }
                color: Material.color(Material.BlueGrey, Material.Shade900)
                text: model.value // Roles::ValueRole

                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        console.debug("@@@@@ authorizationPanel.loginComboBox.onClicked ", index)
                        app.onUserAction();
                        loginComboBox.currentIndex = index
                        loginComboBox.displayText = text
                        loginComboBox.popup.close()
                        passwordTextField.forceActiveFocus()
                    }
                }
            }
        }

        Text
        {
            Layout.column: 0
            Layout.row: 4
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: screenManager.spacer()
            font { pointSize: screenManager.normalFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: qsTr("Пароль")
        }

        TextField
        {
            id: passwordTextField
            Layout.column: 0
            Layout.row: 5
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.preferredWidth: screenManager.editWidth()
            font { pointSize: screenManager.normalFontSize() }
            Material.accent: Material.Orange
            color: Material.color(Material.BlueGrey, Material.Shade900)
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ passwordTextField Keys.onPressed ", JSON.stringify(event))
                app.onUserAction();
                event.accepted = true;
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
                        app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text)
                        break
                    default:
                        app.beep();
                        break
                }
            }
        }

        RoundTextButton
        {
            Layout.column: 0
            Layout.row: 6
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: screenManager.spacer() * 2
            text: qsTr("ПРОДОЛЖИТЬ")
            onClicked: app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text)
        }
    }
}


