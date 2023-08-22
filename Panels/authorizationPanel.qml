import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: authorizationPanel
    padding : 0
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
            app.onUserAction(); // AppManager's slot
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

    Column
    {
        anchors.fill: parent

        Text
        {
            anchors.horizontalCenter: parent.horizontalCenter
            topPadding: Constants.margin
            font { pointSize: Constants.largeFontSize }
            color: Material.color(Material.BlueGrey, Material.Shade400)
            text: qsTr("Штрих ПРИНТ 6А")
        }

        Column
        {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            Text
            {
                anchors.horizontalCenter: parent.horizontalCenter
                bottomPadding: Constants.margin
                font { pointSize: Constants.largeFontSize }
                color: Material.color(Material.BlueGrey, Material.Shade600)
                text: qsTr("Авторизация")
            }

            Text
            {
                font { pointSize: Constants.normalFontSize }
                color: Material.color(Material.BlueGrey, Material.Shade600)
                text: qsTr("Имя")
            }

            ComboBox
            {
                id: loginComboBox
                width: Constants.inputFieldWidth
                editable: false
                popup.modal: true

                model: userNameModel
                delegate: Text
                {
                    padding: Constants.margin
                    font { pointSize: Constants.normalFontSize }
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    text: model.value // Roles::ValueRole

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            console.debug("@@@@@ authorizationPanel.loginComboBox.onClicked ", index)
                            app.onUserAction(); // AppManager's slot
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
                topPadding: Constants.margin
                font { pointSize: Constants.normalFontSize }
                color: Material.color(Material.BlueGrey, Material.Shade600)
                text: qsTr("Пароль")
            }

            TextField
            {
                id: passwordTextField
                width: Constants.inputFieldWidth
                bottomPadding: Constants.margin * 2
                font { pointSize: Constants.normalFontSize }
                Material.accent: Material.Orange
                color: Material.color(Material.BlueGrey, Material.Shade900)
                placeholderText: "?????"
                //inputMethodHints: Qt.ImhDigitsOnly // Keyboard

                focus: true
                Keys.onPressed: (event) =>
                {
                    console.debug("@@@@@ passwordTextField Keys.onPressed ", JSON.stringify(event))
                    event.accepted = true;
                    app.onUserAction(); // AppManager's slot
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
                        case Qt.Key_Backspace:
                        case Qt.Key_Delete:
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

            RoundTextButton
            {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("ПРОДОЛЖИТЬ")
                onClicked:
                {
                    app.onUserAction(); // AppManager's slot
                    app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text) // AppManager's slot
                }
            }
        }

        Text
        {
            id: versionText
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            bottomPadding: Constants.margin
            color: Material.color(Material.BlueGrey, Material.Shade600)
            font { pointSize: Constants.normalFontSize }
            text: versionValue
        }
    }
}

