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
        radius: app.spacer()
        Material.background: Material.color(Material.Grey, Material.Shade100)
        color: Material.background

        GridLayout
        {
            anchors.fill: parent
            anchors.margins: app.spacer()
            columnSpacing: app.spacer()
            rowSpacing: app.spacer()
            columns: 3
            rows: 3

            Rectangle
            {
                Layout.column: 0
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                Layout.preferredWidth: app.buttonSize()
                Layout.preferredHeight: app.buttonSize()
                color: "transparent"
            }

            Rectangle
            {
                Layout.column: 1
                Layout.row: 0
                Layout.fillWidth: parent
                Layout.preferredHeight: app.buttonSize()
                color: "transparent"

                CardTitleText
                {
                    text: "Количество товара"
                }
            }

            RoundIconButton
            {
                Layout.column: 2
                Layout.row: 0
                Layout.alignment: Qt.AlignTop | Qt.AlignRigth
                icon.source: "../Icons/close_black_48"
                onClicked:
                {
                    app.onUserAction(); // AppManager's slot
                    app.onPiecesInputClosed(inputPiecesPanelText.text) // AppManager's slot
                    inputPiecesPanel.close()
                }
            }

            Rectangle
            {
                Layout.column: 0
                Layout.row: 1
                Layout.columnSpan: 3
                Layout.fillWidth: parent
                Layout.fillHeight: parent
                Layout.bottomMargin: app.buttonSize() / 2
                color: "transparent"

                TextField
                {
                    id: inputPiecesPanelText
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: app.editWidth() / 4
                    font { pointSize: app.normalFontSize() }
                    Material.accent: Material.Orange
                    color: Material.color(Material.BlueGrey, Material.Shade900)
                    focus: true
                    text: inputText

                    Keys.onPressed: (event) =>
                    {
                        console.debug("@@@@@ inputPiecesPanelText Keys.onPressed ", JSON.stringify(event))
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
                                app.onPiecesInputClosed(text)
                                inputPiecesPanel.close()
                                break
                             default:
                                app.onBeep();
                                break
                        }
                    }
                }
            }

            Row
            {
                Layout.column: 1
                Layout.row: 2
                Layout.preferredWidth: app.buttonSize() * 2 + spacing
                Layout.preferredHeight: app.buttonSize()
                Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
                spacing: app.spacer() * 4

                RoundTextButton
                {
                    width: app.buttonSize()
                    font { pointSize: app.largeFontSize() }
                    text: qsTr("-")
                    onClicked:
                    {
                        app.onUserAction(); // AppManager's slot
                        if (parseInt(inputPiecesPanelText.text) > 1)
                            inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) - 1
                    }
                }

                RoundTextButton
                {
                    width: app.buttonSize()
                    font { pointSize: app.largeFontSize() }
                    text: qsTr("+")
                    onClicked:
                    {
                        app.onUserAction(); // AppManager's slot
                        inputPiecesPanelText.text = parseInt(inputPiecesPanelText.text) + 1
                    }
                }
            }
        }
    }
}
