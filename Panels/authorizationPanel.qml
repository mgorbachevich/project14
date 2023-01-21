import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants

Popup
{
    id: authorizationPanel
    padding : 0
    closePolicy: Popup.NoAutoClose
    focus: true
    modal: true
    dim: true

    Column
    {
        anchors.centerIn: parent

        Label
        {
            font { pointSize: Constants.smallFontSize; family:'Roboto'; styleName:'Regular' }
            color: Material.color(Material.Grey, Material.Shade400)
            text: qsTr("Имя")
        }

        TextField
        {
            id: loginTextField
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Regular'  }
            placeholderText: "?????"
            inputMethodHints: Qt.ImhDigitsOnly // Keyboard
        }

        Label
        {
            anchors.topMargin: Constants.margin
            font { pointSize: Constants.smallFontSize; family:'Roboto'; styleName:'Regular' }
            color: Material.color(Material.Grey, Material.Shade400)
            text: qsTr("Пароль")
        }

        TextField
        {
            id: passwordTextField
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Regular' }
            placeholderText: "?????"
            inputMethodHints: Qt.ImhDigitsOnly // Keyboard
        }

        Button
        {
            Material.background: Material.accent
            text: qsTr("ПРОДОЛЖИТЬ")
            onClicked:
            {
                app.onAuthorizationPanelClosed(loginTextField.text, passwordTextField.text) // AppManager's slot
                authorizationPanel.close()
            }
        }
    }
}

