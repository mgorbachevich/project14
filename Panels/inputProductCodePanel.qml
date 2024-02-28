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
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Connections // Slot for signal AppManager::closeInputProductPanel
    {
        target: app
        function onCloseInputProductPanel()
        {
            inputProductCodePanel.close()
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
            rowSpacing: 0
            columns: 3
            rows: 4

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
                Layout.fillHeight: parent
                color: "transparent"

                TextField
                {
                    id: inputProductCodePanelText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: screenManager.editWidth()
                    font { pointSize: screenManager.normalFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText
                    onTextChanged:
                    {
                        console.debug("@@@@@ inputProductCodePanelText onTextChanged ", text)
                        if (parseInt(text) <= 0) text = "";
                    }
                    Keys.onPressed: (event) =>
                    {
                        console.debug("@@@@@ inputProductCodePanelText Keys.onPressed ", JSON.stringify(event))
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
                                app.onProductCodeInput(text);
                                break;
                             default:
                                app.beepSound();
                                break;
                        }
                    }
                }
            }

            RoundTextButton
            {
                Layout.column: 1
                Layout.row: 2
                Layout.bottomMargin: screenManager.spacer()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                text: qsTr("ПРОДОЛЖИТЬ")
                onClicked: app.onProductCodeInput(inputProductCodePanelText.text)
            }
        }
    }
}
