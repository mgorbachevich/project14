import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
import RegisteredTypes

Popup
{
    id: inputProductCodePanel
    padding: 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string inputText: "Input"

    Connections // Slot for signal AppManager::closeInputProductPanel
    {
        target: app
        function onCloseInputProductPanel()
        {
            inputProductCodePanel.close()
        }
    }

    Connections // Slot for signal AppManager::enableSetProductByInputCode
    {
        target: app
        function onEnableSetProductByInputCode(value)
        {
            inputProductCodePanelContinueButton.enabled = value
        }
    }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: screenManager.spacer() * 2
            columns: 3

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
                Layout.fillWidth: parent
                Layout.alignment: Qt.AlignCenter
                color: "transparent"

                CardTitleText { text: "Код товара" }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close"
                onClicked: inputProductCodePanel.close()
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 1
                Layout.fillWidth: parent
                color: "transparent"

                TextField
                {
                    id: inputProductCodePanelText
                    anchors.verticalCenter: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: screenManager.editWidth()
                    font { pointSize: screenManager.normalFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText
                    onTextEdited: app.onUserAction();
                    onTextChanged:
                    {
                        if (parseInt(text) <= 0) text = "";
                        app.debugLog("@@@@@ inputProductCodePanelText onTextChanged %1".arg(text))
                        app.onProductCodeEdited(text);
                    }
                    Keys.onPressed: (event) =>
                    {
                        app.debugLog("@@@@@ inputProductCodePanelText Keys.onPressed %1".arg(JSON.stringify(event)))
                        event.accepted = true;
                        app.clickSound();
                        app.onUserAction();
                        switch (event.key)
                        {
                            case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                            case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                                text += event.text
                                break;
                            case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                                if(text.length > 0) text = text.substring(0, text.length - 1);
                                break;
                            case Qt.Key_Escape:
                                inputProductCodePanel.close();
                                break;
                            case Qt.Key_Enter: case Qt.Key_Return:
                                if (inputProductCodePanelContinueButton.enabled) app.onSetProductByCodeClicked(text);
                                break;
                             default:
                                app.beepSound();
                                break;
                        }
                    }
                }
            }

            Rectangle
            {
                id: inputProductCodePanelListRectangle
                Layout.column: 1
                Layout.row: 2
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                Layout.topMargin: screenManager.spacer() * 2
                color: Material.color(Material.Grey, Material.Shade200)

                ListView
                {
                    id: inputProductCodePanelList
                    anchors.fill: parent
                    orientation: Qt.Vertical
                    clip: true

                    ScrollBar.vertical: ScrollBar
                    {
                        width: screenManager.scrollBarWidth()
                        background: Rectangle { color: "transparent" }
                        policy: ScrollBar.AlwaysOn
                    }

                    model: inputProductCodePanelModel
                    delegate: Label
                    {
                        width: inputProductCodePanelListRectangle.width
                        font { pointSize: screenManager.normalFontSize() }
                        color: Material.color(Material.Grey, Material.Shade900)
                        text: model.value // Roles::ValueRole
                        background: Rectangle { color: "transparent" }
                    }
                }
            }

            RoundTextButton
            {
                id: inputProductCodePanelContinueButton
                Layout.column: 1
                Layout.row: 3
                Layout.bottomMargin: screenManager.spacer()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                text: qsTr("ПРОДОЛЖИТЬ")
                onClicked: app.onSetProductByCodeClicked(inputProductCodePanelText.text)
            }
        }
    }
}
