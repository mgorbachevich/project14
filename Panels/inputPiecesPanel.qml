import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import RegisteredTypes


Popup
{
    id: inputPiecesPanel
    padding: 0
    //closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    closePolicy: Popup.CloseOnEscape
    focus: true
    modal: true
    dim: true
    Material.background: "transparent"
    property string inputText: ""
    property int maxChars: 2
    property bool clicked: false
    onOpened: app.onPopupOpened(true)
    onClosed:
    {
        app.onPiecesInputClosed(inputPiecesPanelText.text)
        app.onPopupOpened(false)
    }

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.color(Material.Grey, Material.Shade100)

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: screenManager.spacer()
            columnSpacing: 0
            rowSpacing: 0

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

                CardTitleText { text: "Количество товара" }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close"
                onClicked: inputPiecesPanel.close()
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
                    id: inputPiecesPanelText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: screenManager.editWidth() / 4
                    font { pointSize: screenManager.largeFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText
                    onTextEdited: app.onUserAction();
                    onTextChanged:
                    {
                        if (inputPiecesPanelText.text.length > maxChars)
                        {
                            app.beepSound();
                            inputPiecesPanelText.text = inputPiecesPanelText.text.substring(0, inputPiecesPanelText.text.length - 1);
                            app.showAttention("Максимальная длина " + maxChars)
                        }
                        if (parseInt(inputPiecesPanelText.text) <= 0) inputPiecesPanelText.text = "";
                    }
                    Keys.onPressed: (event) =>
                    {
                        //app.debugLog("@@@@@ inputPiecesPanelText Keys.onPressed %1".arg(JSON.stringify(event)))
                        event.accepted = true;
                        app.clickSound();
                        app.onUserAction();
                        switch (event.key)
                        {
                            case Qt.Key_0: case Qt.Key_1: case Qt.Key_2: case Qt.Key_3: case Qt.Key_4:
                            case Qt.Key_5: case Qt.Key_6: case Qt.Key_7: case Qt.Key_8: case Qt.Key_9:
                                if(clicked) text += event.text
                                else text = event.text
                                inputPiecesPanel.clicked = true
                                break;
                            case Qt.Key_Backspace: case Qt.Key_Delete: case Qt.Key_C:
                                if(text.length > 0) text = text.substring(0, text.length - 1);
                                else app.beepSound();
                                inputPiecesPanel.clicked = true
                                break;
                            case Qt.Key_F10: // Промотка
                                app.onRewind()
                                break
                            case Qt.Key_Escape:
                                text = ""
                                inputPiecesPanel.clicked = true
                                break;
                            case Qt.Key_Enter: case Qt.Key_Return:
                                inputPiecesPanel.close();
                                break;
                             default:
                                app.beepSound();
                                break;
                        }
                        inputPiecesPanelText.focus = true
                    }
                }
            }

            Row
            {
                Layout.column: 1
                Layout.row: 2
                Layout.preferredWidth: screenManager.buttonSize() * 2 + spacing
                Layout.preferredHeight: screenManager.buttonSize()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                spacing: screenManager.spacer() * 4

                RoundTextButton
                {
                    width: screenManager.buttonSize()
                    font { pointSize: screenManager.largeFontSize() }
                    text: qsTr("-")
                    onClicked:
                    {
                        app.onUserAction();
                        if(inputPiecesPanelText.text !== "" && parseInt(inputPiecesPanelText.text) > 0)
                            inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) - 1
                        else app.beepSound();
                        inputPiecesPanelText.focus = true
                        inputPiecesPanel.clicked = true
                    }
                }

                RoundTextButton
                {
                    width: screenManager.buttonSize()
                    font { pointSize: screenManager.largeFontSize() }
                    text: qsTr("+")
                    onClicked:
                    {
                        app.onUserAction();
                        if(inputPiecesPanelText.text === "") inputPiecesPanelText.text = "1"
                        else inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) + 1
                        inputPiecesPanelText.focus = true
                        inputPiecesPanel.clicked = true
                    }
                }
            }
        }
    }
}
