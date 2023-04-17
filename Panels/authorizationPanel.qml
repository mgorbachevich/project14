import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes 1.0

Popup
{
    id: authorizationPanel
    objectName: "authorizationPanel"
    padding : 0
    closePolicy: Popup.NoAutoClose
    focus: true
    modal: true
    dim: true
    property string versionValue: "Version"
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::authorizationSucceded:
    {
        target: app
        function onAuthorizationSucceded() { authorizationPanel.close() }
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

    Text
    {
        id: versionText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.Top
        topPadding: Constants.margin
        font { pointSize: Constants.tinyFontSize; family:'Roboto'; styleName:'Regular' }
        color: Material.color(Material.Grey, Material.Shade400)
        text: versionValue
    }

    Column
    {
        anchors.centerIn: parent

        Text
        {
            anchors.horizontalCenter: parent.horizontalCenter
            font { pointSize: Constants.largeFontSize; family:'Roboto'; styleName:'Bold' }
            text: qsTr("Авторизация")
        }

        Text
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
                        passwordTextField.forceActiveFocus()
                    }
                }
            }
        }

        Text
        {
            font { pointSize: Constants.smallFontSize; family:'Roboto'; styleName:'Regular' }
            color: Material.color(Material.Grey, Material.Shade400)
            text: qsTr("Пароль")
        }

        TextField
        {
            id: passwordTextField
            font { pointSize: Constants.normalFontSize; family: "Roboto"; styleName:'Regular' }
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            //width: Constants.inputFieldWidth

            focus: true
            Keys.onPressed: (event) =>
            {
                console.debug("@@@@@ passwordTextField Keys.onPressed ", JSON.stringify(event))
                event.accepted = true;
                switch (event.key)
                {
                    case Qt.Key_0:
                    case Qt.Key_1:
                    case Qt.Key_2:
                    case Qt.Key_3:
                    case Qt.Key_4:
                    case Qt.Key_5:
                    case Qt.Key_6:
                    case Qt.Key_7:
                    case Qt.Key_8:
                    case Qt.Key_9:
                        text += event.text
                        break;
                    case Qt.Key_C:
                        text = text.substring(0, text.length - 1);
                        break;
                    case Qt.Key_Escape:
                        text = ""
                        break;
                    case Qt.Key_Enter:
                        app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text)
                        break
                    default:
                        app.onBeep();
                        break
                }
            }
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

