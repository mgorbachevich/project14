import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import "../constants.js" as Constants
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
    property string inputText: "Input"
    onOpened: app.onPopupOpened()
    onClosed: app.onPopupClosed()

    Rectangle
    {
        anchors.fill: parent
        radius: screenManager.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            id: inputPiecesPanelLayout
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

                CardTitleText { text: "Количество товара" }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close_black_48"
                onClicked:
                {
                    app.onPiecesInputClosed(inputPiecesPanelText.text)
                    inputPiecesPanel.close()
                }
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
                    font { pointSize: screenManager.normalFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText
                    Keys.onPressed: (event) =>
                    {
                        console.debug("@@@@@ inputPiecesPanelText Keys.onPressed ", JSON.stringify(event))
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
                                text = text.substring(0, text.length - 1);
                                break;
                            case Qt.Key_Escape:
                                text = ""
                                break;
                            case Qt.Key_Enter: case Qt.Key_Return:
                                app.onPiecesInputClosed(text)
                                inputPiecesPanel.close()
                                break
                             default:
                                app.beepSound();
                                break
                        }
                        if (parseInt(text) < 0) text = ""
                    }
                }
            }

            Row
            {
                Layout.column: 1
                Layout.row: 2
                Layout.preferredWidth: screenManager.buttonSize() * 2 + spacing
                Layout.preferredHeight: screenManager.buttonSize()
                Layout.bottomMargin: screenManager.spacer()
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
                        inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) - 1
                        if (parseInt(inputPiecesPanelText.text) < 0) inputPiecesPanelText.text = 0
                        inputPiecesPanelText.focus = true
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
                        inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) + 1
                        inputPiecesPanelText.focus = true
                    }
                }
            }
        }
    }
}
