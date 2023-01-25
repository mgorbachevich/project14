import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: authorizationPanel
    padding : 0
    closePolicy: Popup.NoAutoClose
    focus: true
    modal: true
    dim: true

    Connections // Slot for signal AppManager::authorizationSucceded:
    {
        target: app
        function onAuthorizationSucceded()
        {
            console.debug("@@@@@ productPanel.onAuthorizationSucceded");
            authorizationPanel.close()
        }
    }

    Column
    {
        anchors.centerIn: parent

        Label
        {
            anchors.horizontalCenter: parent.horizontalCenter
            font { pointSize: Constants.largeFontSize; family:'Roboto'; styleName:'Bold' }
            text: qsTr("Авторизация")
        }

        Label
        {
            font { pointSize: Constants.smallFontSize; family:'Roboto'; styleName:'Regular' }
            color: Material.color(Material.Grey, Material.Shade400)
            text: qsTr("Имя")
        }

        ComboBox
        {
            id: loginComboBox
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Regular'  }
            editable: false
            popup.modal: true
            width: Constants.inputFieldWidth

            model: userNameModel
            delegate: Text
            {
                padding: Constants.margin
                //font { pointSize: Constants.normalFontSize; family: "Roboto" }
                //color: Material.color(Material.Grey, Material.Shade50)
                text: model.value // Roles::ValueRole

                MouseArea
                {
                    anchors.fill: parent
                    onClicked:
                    {
                        console.debug("@@@@@ authorizationPanel.loginComboBox.onClicked ", index)
                        loginComboBox.currentIndex = index
                        loginComboBox.displayText = text
                        loginComboBox.popup.close()
                    }
                }
            }
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
            width: Constants.inputFieldWidth
        }

        Button
        {
            anchors.horizontalCenter: parent.horizontalCenter
            Material.background: Material.primary
            text: qsTr(" ПРОДОЛЖИТЬ ")
            onClicked: app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text) // AppManager's slot
        }
    }
}

