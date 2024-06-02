import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Rectangle
{
    id: authorizationPanel
    anchors.fill: parent
    Material.background: Material.color(Material.Grey, Material.Shade100)
    color: Material.background
    property int flagSize: screenManager.flagSize()

    Connections // Slot for signal AppManager::showControlParam:
    {
        target: app
        function onShowControlParam(param, value)
        {
            switch(param)
            {
            case 18:
                dateTimeText.text = value
                break
            case 21:
                authorizationPanelTitle1.text = value
                break
            case 22:
                authorizationPanelTitle2.text = value
                break
            case 23:
                authorizationPanelTitle3.text = value
                break
            }
        }
    }

    Connections // Slot for signal AppManager::showEnvironmentStatus
    {
        target: app
        function onShowEnvironmentStatus(value1, value2, value3, value4)
        {
            //app.debugLog("@@@@@ authorizationPanel.onShowEnvironmentStatus");
            if(value1) usbIcon.source = "../Icons/usb";
            else       usbIcon.source = "../Icons/usb_white"
            if(value2) bluetoothIcon.source = "../Icons/bluetooth";
            else       bluetoothIcon.source = "../Icons/bluetooth_white";
            if(value3) wifiIcon.source = "../Icons/wifi";
            else       wifiIcon.source = "../Icons/wifi_white";
            if(value4) sdcardIcon.source = "../Icons/sdcard";
            else       sdcardIcon.source = "../Icons/sdcard_white";
        }
    }

    Connections // Slot for signal AppManager::showCurrentUser:
    {
        target: app
        function onShowCurrentUser(index, name)
        {
            //app.debugLog("@@@@@ authorizationPanel.onShowCurrentUser")
            loginComboBox.currentIndex = index
            loginComboBox.displayText = name
        }
    }

    Connections // Slot for signal AppManager::showMainPage:
    {
        target: app
        function onShowMainPage(index)
        {
            if (index === -1)
            {
                //app.debugLog("@@@@@ authorizationPanel onShowMainPage");
                passwordTextField.text = ""
                passwordTextField.forceActiveFocus()
            }
        }
    }

    GridLayout
    {
        anchors.fill: parent
        anchors.margins: screenManager.spacer()
        columnSpacing: 0
        rowSpacing: 0

        Image
        {
            Layout.column: 0
            Layout.row: 0
            Layout.alignment: parent.left
            Layout.preferredWidth: screenManager.buttonSize()
            Layout.preferredHeight: screenManager.buttonSize()
            source: "../Images/logo"

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    //app.debugLog("@@@@@ searchPanelTextField on clicked image")
                    app.onInfoClicked()
                }
            }
        }

        Rectangle
        {
            Layout.column: 1
            Layout.row: 0
            Layout.fillWidth: parent
            Layout.preferredHeight: screenManager.buttonSize()
            color: "transparent"

            Row
            {
                anchors.right: parent.right
                spacing: screenManager.spacer()
                visible: true

                Text
                {
                    id: dateTimeText
                    anchors.verticalCenter: parent.verticalCenter
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    font { pointSize: screenManager.normalFontSize() }
                }

                Image
                {
                    id: usbIcon
                    width: screenManager.flagSize()
                    height: screenManager.flagSize()
                    source: "../Icons/usb"
                }

                Image
                {
                    id: bluetoothIcon
                    width: screenManager.flagSize()
                    height: screenManager.flagSize()
                    source: "../Icons/bluetooth"
                }

                Image
                {
                    id: wifiIcon
                    width: screenManager.flagSize()
                    height: screenManager.flagSize()
                    source: "../Icons/wifi"
                }

                Image
                {
                    id: sdcardIcon
                    width: screenManager.flagSize()
                    height: screenManager.flagSize()
                    source: "../Icons/sdcard"
                }
            }
        }

        Rectangle
        {
            Layout.column: 0
            Layout.row: 1
            Layout.columnSpan: 2
            Layout.fillHeight: parent
            Layout.fillWidth: parent
            color: "transparent"

            Column
            {
                id: authorizationPanelLayout
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 0

                Rectangle
                {
                    width: parent.width
                    height: screenManager.normalFontSize()
                    color: "transparent"

                    CardText
                    {
                        id: authorizationPanelTitle1
                        color: Material.color(Material.BlueGrey, Material.Shade600)
                    }
                }

                Rectangle
                {
                    width: parent.width
                    height: screenManager.buttonSize()
                    color: "transparent"

                    CardTitleText { text: qsTr("Авторизация") }
                }

                SubtitleText { text: qsTr("Пользователь") }

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
                            //app.debugLog("@@@@@ authorizationPanel.loginComboBox.onClicked %1".arg(index))
                            app.onUserAction();
                            loginComboBox.currentIndex = index
                            loginComboBox.displayText = text
                            loginComboBox.popup.close()
                            passwordTextField.forceActiveFocus()
                        }
                    }
                    }
                }

                Spacer {}

                SubtitleText { text: qsTr("Пароль") }

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
                        //app.debugLog("@@@@@ authorizationPanel.passwordTextField Keys.onPressed %1".arg(JSON.stringify(event)))
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
                            case Qt.Key_F10: // Промотка
                                app.onRewind()
                                break
                            default:
                                app.beepSound();
                                break
                        }
                    }
                }

                Spacer {}

                RoundTextButton
                {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("ПРОДОЛЖИТЬ")
                    onClicked: app.onCheckAuthorizationClicked(loginComboBox.displayText, passwordTextField.text)
                }
            }
        }

        Row
        {
            Layout.column: 0
            Layout.row: 2
            Layout.columnSpan: 2
            Layout.fillWidth: parent
            Layout.preferredHeight: screenManager.normalFontSize()

            CardText
            {
                id: authorizationPanelTitle2
                anchors.horizontalCenter: parent.left
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                horizontalAlignment: Text.AlignLeft
                color: Material.color(Material.BlueGrey, Material.Shade600)
            }

            CardText
            {
                id: authorizationPanelTitle3
                anchors.horizontalCenter: parent.right
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                horizontalAlignment: Text.AlignRight
                color: Material.color(Material.BlueGrey, Material.Shade600)
            }
        }
    }
}



