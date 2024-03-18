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
    property int flagSize: screenManager.flagSize()
    onOpened:
    {
        app.onAuthorizationOpened(true)
        app.onPopupOpened()
    }
    onClosed:
    {
        app.onAuthorizationOpened(false)
        app.onPopupClosed()
    }

    Connections // Slot for signal AppManager::showEnvironmentStatus
    {
        target: app
        function onShowEnvironmentStatus(value1, value2, value3, value4)
        {
            app.debugLog("@@@@@ authorizationPanel.onShowEnvironmentStatus");
            if(value1) usbIcon.Material.foreground = Constants.colorBlack;
            else       usbIcon.Material.foreground = Material.color(Material.BlueGrey, Material.Shade200)
            if(value2) bluetoothIcon.Material.foreground = Constants.colorBlack;
            else       bluetoothIcon.Material.foreground = Material.color(Material.BlueGrey, Material.Shade200)
            if(value3) wifiIcon.Material.foreground = Constants.colorBlack;
            else       wifiIcon.Material.foreground = Material.color(Material.BlueGrey, Material.Shade200)
            if(value4) sdcardIcon.Material.foreground = Constants.colorBlack;
            else       sdcardIcon.Material.foreground = Material.color(Material.BlueGrey, Material.Shade200)
        }
    }

    Connections // Slot for signal AppManager::showDateTime:
    {
        target: app
        function onShowDateTime(value)
        {
            app.debugLog("@@@@@ authorizationPanel.onShowDateTime")
            dateTimeText.text = value
        }
    }

    Connections // Slot for signal AppManager::showAuthorizationSucceded:
    {
        target: app
        function onShowAuthorizationSucceded()
        {
            app.debugLog("@@@@@ authorizationPanel.onShowAuthorizationSucceded")
            app.onUserAction();
            authorizationPanel.close()
        }
    }

    Connections // Slot for signal AppManager::setCurrentUser:
    {
        target: app
        function onSetCurrentUser(index, name)
        {
            app.debugLog("@@@@@ authorizationPanel.onSetCurrentUser")
            loginComboBox.currentIndex = index
            loginComboBox.displayText = name
        }
    }

    Image
    {
        anchors.left: parent.left
        width: screenManager.buttonSize() * 3 / 2
        height: screenManager.buttonSize() * 3 / 2
        source: "../Images/logo"

        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                app.debugLog("@@@@@ searchPanelTextField on clicked image")
                app.onInfoClicked()
            }
        }
    }

    Row
    {
        id: flags
        anchors.right: parent.right
        spacing: screenManager.spacer()
        visible: true

        Text
        {
            id: versionText
            color: Material.color(Material.BlueGrey, Material.Shade900)
            font { pointSize: screenManager.normalFontSize() }
            text: versionValue
            visible: false // todo
        }

        Text
        {
            id: dateTimeText
            anchors.verticalCenter: parent.verticalCenter
            color: Material.color(Material.BlueGrey, Material.Shade900)
            font { pointSize: screenManager.normalFontSize() }
        }

        SmallIconButton
        {
            id: usbIcon
            icon.source: "../Icons/usb"
        }

        SmallIconButton
        {
            id: bluetoothIcon
            icon.source: "../Icons/bluetooth"
        }

        SmallIconButton
        {
            id: wifiIcon
            icon.source: "../Icons/wifi"
        }

        SmallIconButton
        {
            id: sdcardIcon
            icon.source: "../Icons/sdcard"
        }
    }

    Column
    {
        id: authorizationPanelLayout
        anchors.centerIn: parent
        spacing: 0

        Text
        {
            horizontalAlignment: Text.AlignHCenter
            width: screenManager.editWidth()
            font { pointSize: screenManager.largeFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade900)
            text: qsTr("Авторизация")
        }

        Text
        {
            font { pointSize: screenManager.largeFontSize() }
            text: " "
        }

        Text
        {
            font { pointSize: screenManager.normalFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: qsTr("Пользователь")
        }

        ComboBox
        {
            id: loginComboBox
            width: screenManager.editWidth()
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
                        app.debugLog("@@@@@ authorizationPanel.loginComboBox.onClicked %1".arg(index))
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
            font { pointSize: screenManager.normalFontSize() }
            text: " "
        }

        Text
        {
            font { pointSize: screenManager.normalFontSize() }
            color: Material.color(Material.BlueGrey, Material.Shade600)
            text: qsTr("Пароль")
        }

        TextField
        {
            id: passwordTextField
            width: screenManager.editWidth()
            font { pointSize: screenManager.normalFontSize() }
            Material.accent: Material.Orange
            color: Material.color(Material.BlueGrey, Material.Shade900)
            placeholderText: "?????"
            //inputMethodHints: Qt.ImhDigitsOnly // Keyboard
            focus: true

            Keys.onPressed: (event) =>
            {
                app.debugLog("@@@@@ authorizationPanel.passwordTextField Keys.onPressed %1".arg(JSON.stringify(event)))
                app.clickSound();
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
                        app.beepSound();
                        break
                }
            }
        }

        Text
        {
            font { pointSize: screenManager.largeFontSize() }
            text: " "
        }

        RoundTextButton
        {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("ПРОДОЛЖИТЬ")
            onClicked: app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text)
        }
    }
}


